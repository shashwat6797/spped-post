#include "ui/MainWindowController.h"

#include "core/DatabaseManager.h"
#include "modules/folder/FolderService.h"
#include "modules/request/HttpService.h"
#include "modules/request/RequestService.h"
#include "ui/RequestWorkspaceWidget.h"
#include "ui/SidebarWidget.h"

#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QStatusBar>

MainWindowController::MainWindowController(
    SidebarWidget *sidebar,
    RequestWorkspaceWidget *workspace,
    FolderService *folderService,
    RequestService *requestService,
    HttpService *httpService,
    QStatusBar *statusBar,
    QWidget *dialogParent,
    QObject *parent
)
    : QObject(parent)
    , m_sidebar(sidebar)
    , m_workspace(workspace)
    , m_folderService(folderService)
    , m_requestService(requestService)
    , m_httpService(httpService)
    , m_statusBar(statusBar)
    , m_dialogParent(dialogParent)
{
}

void MainWindowController::start()
{
    connectRequestFlow();
    connectPersistenceFlow();

    if (DatabaseManager::instance().initialize()) {
        loadSavedData();
        showStatus(QStringLiteral("Ready"));
    } else {
        showStatus(QStringLiteral("Database error: %1").arg(DatabaseManager::instance().lastError()));
    }
}

void MainWindowController::connectRequestFlow()
{
    connect(m_workspace, &RequestWorkspaceWidget::sendRequested, m_httpService, &HttpService::sendGet);
    connect(m_workspace, &RequestWorkspaceWidget::statusMessageChanged, this, &MainWindowController::showStatus);

    connect(m_httpService, &HttpService::requestStarted, m_workspace, &RequestWorkspaceWidget::showSending);
    connect(m_httpService, &HttpService::responseReceived, m_workspace, &RequestWorkspaceWidget::showResponse);
    connect(m_httpService, &HttpService::requestFailed, m_workspace, &RequestWorkspaceWidget::showFailure);
}

void MainWindowController::connectPersistenceFlow()
{
    connect(m_sidebar, &SidebarWidget::addRequestRequested, this, &MainWindowController::createRequestInSelectedFolder);
    connect(m_sidebar, &SidebarWidget::createFolderRequested, this, &MainWindowController::createFolder);
    connect(m_sidebar, &SidebarWidget::renameFolderRequested, this, &MainWindowController::renameSelectedFolder);
    connect(m_sidebar, &SidebarWidget::deleteFolderRequested, this, &MainWindowController::deleteSelectedFolder);
    connect(m_sidebar, &SidebarWidget::deleteRequestRequested, this, &MainWindowController::deleteSelectedRequest);
    connect(m_sidebar, &SidebarWidget::folderSelected, m_workspace, &RequestWorkspaceWidget::setCurrentFolderId);
    connect(m_sidebar, &SidebarWidget::requestSelected, m_workspace, &RequestWorkspaceWidget::loadRequest);
    connect(m_workspace, &RequestWorkspaceWidget::saveRequested, this, &MainWindowController::saveCurrentRequest);
}

void MainWindowController::loadSavedData()
{
    const int currentRequestId = m_sidebar->selectedRequestId();
    const int currentFolderId = m_sidebar->selectedFolderId();
    const auto folders = m_folderService->allFolders();
    const auto requests = m_requestService->allRequests();

    m_workspace->setFolders(folders);
    m_sidebar->setItems(folders, requests);
    m_sidebar->selectRequest(currentRequestId);

    if (m_sidebar->selectedRequestId() < 0) {
        m_sidebar->selectFolder(currentFolderId);
    }
}

void MainWindowController::createFolder()
{
    bool ok = false;
    const QString name = QInputDialog::getText(
        m_dialogParent,
        QStringLiteral("Create Folder"),
        QStringLiteral("Folder name"),
        QLineEdit::Normal,
        QString(),
        &ok
    );

    if (!ok) {
        return;
    }

    const int folderId = m_folderService->createFolder(name);
    if (folderId < 0) {
        showStatus(QStringLiteral("Create folder failed: %1").arg(m_folderService->lastError()));
        return;
    }

    loadSavedData();
    m_sidebar->selectFolder(folderId);
    showStatus(QStringLiteral("Folder created"));
}

void MainWindowController::renameSelectedFolder()
{
    const int folderId = m_sidebar->selectedFolderId();
    if (folderId < 0) {
        showStatus(QStringLiteral("Select a folder to rename"));
        return;
    }

    bool ok = false;
    const QString name = QInputDialog::getText(
        m_dialogParent,
        QStringLiteral("Rename Folder"),
        QStringLiteral("Folder name"),
        QLineEdit::Normal,
        m_sidebar->selectedFolderName(),
        &ok
    );

    if (!ok) {
        return;
    }

    if (!m_folderService->renameFolder(folderId, name)) {
        showStatus(QStringLiteral("Rename failed: %1").arg(m_folderService->lastError()));
        return;
    }

    loadSavedData();
    m_sidebar->selectFolder(folderId);
    showStatus(QStringLiteral("Folder renamed"));
}

void MainWindowController::deleteSelectedFolder()
{
    const int folderId = m_sidebar->selectedFolderId();
    if (folderId < 0) {
        showStatus(QStringLiteral("Select a folder to delete"));
        return;
    }

    const auto response = QMessageBox::question(
        m_dialogParent,
        QStringLiteral("Delete Folder"),
        QStringLiteral("Delete this folder? Contained requests will move to Root.")
    );
    if (response != QMessageBox::Yes) {
        return;
    }

    if (!m_folderService->deleteFolder(folderId)) {
        showStatus(QStringLiteral("Delete folder failed: %1").arg(m_folderService->lastError()));
        return;
    }

    loadSavedData();
    m_sidebar->clearSelection();
    m_workspace->clearRequest();
    showStatus(QStringLiteral("Folder deleted; requests moved to Root"));
}

void MainWindowController::createRequestInSelectedFolder()
{
    const int folderId = m_sidebar->selectedFolderId();
    m_sidebar->clearSelection();
    m_workspace->clearRequest(folderId);
}

void MainWindowController::saveCurrentRequest()
{
    const SavedRequest request = m_workspace->currentDraft(m_sidebar->selectedRequestId());
    const int savedId = m_requestService->saveRequest(request);
    if (savedId < 0) {
        showStatus(QStringLiteral("Save failed: %1").arg(m_requestService->lastError()));
        return;
    }

    loadSavedData();
    m_sidebar->selectRequest(savedId);
    showStatus(QStringLiteral("Request saved"));
}

void MainWindowController::deleteSelectedRequest()
{
    const int id = m_sidebar->selectedRequestId();
    if (id < 0) {
        showStatus(QStringLiteral("Select a request to delete"));
        return;
    }

    if (!m_requestService->deleteRequest(id)) {
        showStatus(QStringLiteral("Delete failed: %1").arg(m_requestService->lastError()));
        return;
    }

    loadSavedData();
    m_sidebar->clearSelection();
    m_workspace->clearRequest();
    showStatus(QStringLiteral("Request deleted"));
}

void MainWindowController::showStatus(const QString &message)
{
    m_statusBar->showMessage(message);
}
