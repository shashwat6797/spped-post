#include "MainWindow.h"

#include "models/Folder.h"
#include "models/Request.h"
#include "models/SavedRequest.h"
#include "services/HttpService.h"
#include "services/RequestRepository.h"

#include <QComboBox>
#include <QFrame>
#include <QHash>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QTableWidget>
#include <QTabWidget>
#include <QToolButton>
#include <QTreeWidget>
#include <QUrl>
#include <QVariant>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_httpService(new HttpService(this))
    , m_requestRepository(new RequestRepository(this))
{
    setWindowTitle(QStringLiteral("Postman App"));
    setMinimumSize(1100, 700);
    resize(1280, 800);

    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setChildrenCollapsible(true);
    splitter->addWidget(createSidebar());
    splitter->addWidget(createRequestWorkspace());
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({280, 1000});

    setCentralWidget(splitter);
    connectRequestFlow();
    connectPersistenceFlow();

    if (m_requestRepository->initialize()) {
        loadSavedData();
        statusBar()->showMessage(QStringLiteral("Ready"));
    } else {
        statusBar()->showMessage(QStringLiteral("Database error: %1").arg(m_requestRepository->lastError()));
    }
}

QWidget *MainWindow::createSidebar()
{
    auto *sidebar = new QFrame(this);
    sidebar->setProperty("role", "sidebar");
    sidebar->setMinimumWidth(240);
    sidebar->setMaximumWidth(360);

    auto *layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    auto *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(8);

    auto *title = new QLabel(QStringLiteral("Saved Requests"), sidebar);
    title->setProperty("role", "title");

    m_addRequestButton = new QPushButton(QStringLiteral("Add Request"), sidebar);
    m_addRequestButton->setProperty("variant", "primary");
    m_addRequestButton->setFixedHeight(32);

    headerLayout->addWidget(title, 1);
    headerLayout->addWidget(m_addRequestButton);

    auto *hint = new QLabel(QStringLiteral("Select a folder or request."), sidebar);
    hint->setProperty("role", "muted");

    m_savedRequestsTree = new QTreeWidget(sidebar);
    m_savedRequestsTree->setFrameShape(QFrame::NoFrame);
    m_savedRequestsTree->setHeaderHidden(true);

    auto *folderActions = new QHBoxLayout();
    folderActions->setSpacing(8);

    m_createFolderButton = new QPushButton(QStringLiteral("Folder"), sidebar);
    m_renameFolderButton = new QPushButton(QStringLiteral("Rename"), sidebar);
    m_deleteFolderButton = new QPushButton(QStringLiteral("Delete Folder"), sidebar);
    m_deleteFolderButton->setProperty("variant", "danger");

    folderActions->addWidget(m_createFolderButton);
    folderActions->addWidget(m_renameFolderButton);

    m_deleteRequestButton = new QPushButton(QStringLiteral("Delete Request"), sidebar);
    m_deleteRequestButton->setProperty("variant", "danger");

    layout->addLayout(headerLayout);
    layout->addWidget(hint);
    layout->addWidget(m_savedRequestsTree, 1);
    layout->addLayout(folderActions);
    layout->addWidget(m_deleteFolderButton);
    layout->addWidget(m_deleteRequestButton);

    return sidebar;
}

QWidget *MainWindow::createRequestWorkspace()
{
    auto *workspace = new QWidget(this);
    auto *layout = new QVBoxLayout(workspace);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(14);

    auto *topBar = new QFrame(workspace);
    topBar->setProperty("role", "toolbar");

    auto *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(12, 10, 12, 10);
    topBarLayout->setSpacing(10);

    m_requestNameLineEdit = new QLineEdit(topBar);
    m_requestNameLineEdit->setPlaceholderText(QStringLiteral("Request name"));
    m_requestNameLineEdit->setText(QStringLiteral("Untitled Request"));

    m_saveRequestButton = new QPushButton(QStringLiteral("Save Request"), topBar);
    auto *moreButton = new QPushButton(QStringLiteral("..."), topBar);
    moreButton->setProperty("variant", "ghost");
    moreButton->setFixedWidth(42);

    topBarLayout->addWidget(m_requestNameLineEdit, 1);
    topBarLayout->addWidget(m_saveRequestButton);
    topBarLayout->addWidget(moreButton);

    auto *requestPanel = new QFrame(workspace);
    requestPanel->setProperty("role", "panel");

    auto *requestLayout = new QVBoxLayout(requestPanel);
    requestLayout->setContentsMargins(16, 16, 16, 16);
    requestLayout->setSpacing(14);

    auto *requestLine = new QHBoxLayout();
    requestLine->setSpacing(10);

    m_methodComboBox = new QComboBox(requestPanel);
    m_methodComboBox->addItem(QStringLiteral("GET"));
    m_methodComboBox->setFixedWidth(110);

    m_folderComboBox = new QComboBox(requestPanel);
    m_folderComboBox->setMinimumWidth(180);

    m_urlLineEdit = new QLineEdit(requestPanel);
    m_urlLineEdit->setProperty("role", "url");
    m_urlLineEdit->setPlaceholderText(QStringLiteral("Enter request URL"));
    m_urlLineEdit->setText(QStringLiteral("https://httpbin.org/get"));

    m_sendButton = new QPushButton(QStringLiteral("Send"), requestPanel);
    m_sendButton->setProperty("variant", "primary");
    m_sendButton->setFixedWidth(96);

    requestLine->addWidget(m_methodComboBox);
    requestLine->addWidget(m_folderComboBox);
    requestLine->addWidget(m_urlLineEdit, 1);
    requestLine->addWidget(m_sendButton);

    auto *tabs = new QTabWidget(requestPanel);

    auto *paramsTable = new QTableWidget(4, 3, tabs);
    paramsTable->setHorizontalHeaderLabels({
        QStringLiteral("Key"),
        QStringLiteral("Value"),
        QStringLiteral("Description")
    });
    paramsTable->horizontalHeader()->setStretchLastSection(true);
    paramsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    paramsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    paramsTable->setAlternatingRowColors(true);

    auto *headersTable = new QTableWidget(4, 3, tabs);
    headersTable->setHorizontalHeaderLabels({
        QStringLiteral("Key"),
        QStringLiteral("Value"),
        QStringLiteral("Description")
    });
    headersTable->horizontalHeader()->setStretchLastSection(true);
    headersTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    headersTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    headersTable->setAlternatingRowColors(true);

    auto *bodyEditor = new QPlainTextEdit(tabs);
    bodyEditor->setPlaceholderText(QStringLiteral("{\n  \"name\": \"Ada Lovelace\"\n}"));

    tabs->addTab(paramsTable, QStringLiteral("Params"));
    tabs->addTab(headersTable, QStringLiteral("Headers"));
    tabs->addTab(bodyEditor, QStringLiteral("Body"));

    requestLayout->addLayout(requestLine);
    requestLayout->addWidget(tabs, 1);

    auto *responsePanel = new QFrame(workspace);
    responsePanel->setProperty("role", "panel");

    auto *responseLayout = new QVBoxLayout(responsePanel);
    responseLayout->setContentsMargins(16, 16, 16, 16);
    responseLayout->setSpacing(10);

    auto *responseHeader = new QHBoxLayout();
    responseHeader->setSpacing(10);

    auto *responseTitle = new QLabel(QStringLiteral("Response"), responsePanel);
    responseTitle->setProperty("role", "title");

    m_responseStatusLabel = new QLabel(QStringLiteral("No response yet"), responsePanel);
    m_responseStatusLabel->setProperty("role", "accent");

    responseHeader->addWidget(responseTitle, 1);
    responseHeader->addWidget(m_responseStatusLabel);

    m_responseBodyViewer = new QPlainTextEdit(responsePanel);
    m_responseBodyViewer->setReadOnly(true);
    m_responseBodyViewer->setPlaceholderText(QStringLiteral("Response body will appear here"));

    responseLayout->addLayout(responseHeader);
    responseLayout->addWidget(m_responseBodyViewer, 1);

    layout->addWidget(topBar);
    layout->addWidget(requestPanel, 3);
    layout->addWidget(responsePanel, 2);

    return workspace;
}

void MainWindow::connectRequestFlow()
{
    connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::sendCurrentRequest);
    connect(m_urlLineEdit, &QLineEdit::returnPressed, this, &MainWindow::sendCurrentRequest);

    connect(m_httpService, &HttpService::requestStarted, this, [this]() {
        setRequestInProgress(true);
        m_responseStatusLabel->setText(QStringLiteral("Sending..."));
        m_responseBodyViewer->clear();
        statusBar()->showMessage(QStringLiteral("Sending request"));
    });

    connect(m_httpService, &HttpService::responseReceived, this, [this](const QString &body) {
        setRequestInProgress(false);
        m_responseStatusLabel->setText(QStringLiteral("Response received"));
        m_responseBodyViewer->setPlainText(body);
        statusBar()->showMessage(QStringLiteral("Request complete"));
    });

    connect(m_httpService, &HttpService::requestFailed, this, [this](const QString &message) {
        setRequestInProgress(false);
        m_responseStatusLabel->setText(QStringLiteral("Request failed"));
        m_responseBodyViewer->setPlainText(message);
        statusBar()->showMessage(QStringLiteral("Request failed"));
    });
}

void MainWindow::connectPersistenceFlow()
{
    connect(m_addRequestButton, &QPushButton::clicked, this, &MainWindow::createRequestInSelectedFolder);
    connect(m_createFolderButton, &QPushButton::clicked, this, &MainWindow::createFolder);
    connect(m_renameFolderButton, &QPushButton::clicked, this, &MainWindow::renameSelectedFolder);
    connect(m_deleteFolderButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedFolder);
    connect(m_saveRequestButton, &QPushButton::clicked, this, &MainWindow::saveCurrentRequest);
    connect(m_deleteRequestButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedRequest);

    connect(m_savedRequestsTree, &QTreeWidget::currentItemChanged, this, [this](QTreeWidgetItem *current) {
        if (current == nullptr) {
            return;
        }

        const int itemType = current->data(0, Qt::UserRole).toInt();
        if (itemType == 1) {
            populateRequestInputs(current->data(0, Qt::UserRole + 1).toInt());
        } else {
            selectFolderInEditor(current->data(0, Qt::UserRole + 1).toInt());
        }
    });
}

void MainWindow::loadSavedData()
{
    const int currentId = selectedRequestId();
    const int currentFolderId = selectedFolderId();
    m_savedRequestsTree->clear();
    populateFolderSelector();

    auto *rootItem = new QTreeWidgetItem(m_savedRequestsTree, {QStringLiteral("Root")});
    rootItem->setData(0, Qt::UserRole, 0);
    rootItem->setData(0, Qt::UserRole + 1, -1);
    rootItem->setExpanded(true);

    QHash<int, QTreeWidgetItem *> folderItems;
    for (const Folder &folder : m_requestRepository->allFolders()) {
        auto *folderItem = new QTreeWidgetItem(m_savedRequestsTree, {folder.name});
        folderItem->setData(0, Qt::UserRole, 0);
        folderItem->setData(0, Qt::UserRole + 1, folder.id);
        folderItem->setExpanded(true);
        folderItems.insert(folder.id, folderItem);
    }

    for (const SavedRequest &request : m_requestRepository->allRequests()) {
        auto *item = new QTreeWidgetItem({
            QStringLiteral("%1  %2").arg(request.method, request.name),
        });
        item->setData(0, Qt::UserRole, 1);
        item->setData(0, Qt::UserRole + 1, request.id);
        item->setData(0, Qt::UserRole + 2, request.folderId);
        item->setData(0, Qt::UserRole + 3, request.name);
        item->setData(0, Qt::UserRole + 4, request.method);
        item->setData(0, Qt::UserRole + 5, request.url);

        auto *parentItem = folderItems.value(request.folderId, rootItem);
        parentItem->addChild(item);
    }

    selectSavedRequest(currentId);
    if (selectedRequestId() < 0) {
        auto *folderItem = findFolderItem(currentFolderId);
        if (folderItem != nullptr) {
            m_savedRequestsTree->setCurrentItem(folderItem);
        }
    }
}

void MainWindow::populateRequestInputs(int id)
{
    if (id < 0) {
        return;
    }

    auto *item = findRequestItem(id);
    if (item == nullptr) {
        return;
    }

    m_requestNameLineEdit->setText(item->data(0, Qt::UserRole + 3).toString());
    m_methodComboBox->setCurrentText(item->data(0, Qt::UserRole + 4).toString());
    m_urlLineEdit->setText(item->data(0, Qt::UserRole + 5).toString());
    selectFolderInEditor(item->data(0, Qt::UserRole + 2).toInt());
    m_responseStatusLabel->setText(QStringLiteral("Loaded saved request"));
    statusBar()->showMessage(QStringLiteral("Loaded saved request"));
}

void MainWindow::populateFolderSelector()
{
    const int currentFolderId = currentEditorFolderId();
    m_folderComboBox->clear();
    m_folderComboBox->addItem(QStringLiteral("Root"), -1);

    for (const Folder &folder : m_requestRepository->allFolders()) {
        m_folderComboBox->addItem(folder.name, folder.id);
    }

    selectFolderInEditor(currentFolderId);
}

void MainWindow::createFolder()
{
    bool ok = false;
    const QString name = QInputDialog::getText(
        this,
        QStringLiteral("Create Folder"),
        QStringLiteral("Folder name"),
        QLineEdit::Normal,
        QString(),
        &ok
    ).trimmed();

    if (!ok || name.isEmpty()) {
        return;
    }

    const int folderId = m_requestRepository->saveFolder(name);
    if (folderId < 0) {
        statusBar()->showMessage(QStringLiteral("Create folder failed: %1").arg(m_requestRepository->lastError()));
        return;
    }

    loadSavedData();
    auto *folderItem = findFolderItem(folderId);
    if (folderItem != nullptr) {
        m_savedRequestsTree->setCurrentItem(folderItem);
    }
    statusBar()->showMessage(QStringLiteral("Folder created"));
}

void MainWindow::renameSelectedFolder()
{
    const int folderId = selectedFolderId();
    if (folderId < 0) {
        statusBar()->showMessage(QStringLiteral("Select a folder to rename"));
        return;
    }

    auto *folderItem = findFolderItem(folderId);
    const QString currentName = folderItem != nullptr ? folderItem->text(0) : QString();
    bool ok = false;
    const QString name = QInputDialog::getText(
        this,
        QStringLiteral("Rename Folder"),
        QStringLiteral("Folder name"),
        QLineEdit::Normal,
        currentName,
        &ok
    ).trimmed();

    if (!ok || name.isEmpty()) {
        return;
    }

    if (!m_requestRepository->renameFolder(folderId, name)) {
        statusBar()->showMessage(QStringLiteral("Rename failed: %1").arg(m_requestRepository->lastError()));
        return;
    }

    loadSavedData();
    auto *renamedItem = findFolderItem(folderId);
    if (renamedItem != nullptr) {
        m_savedRequestsTree->setCurrentItem(renamedItem);
    }
    statusBar()->showMessage(QStringLiteral("Folder renamed"));
}

void MainWindow::deleteSelectedFolder()
{
    const int folderId = selectedFolderId();
    if (folderId < 0) {
        statusBar()->showMessage(QStringLiteral("Select a folder to delete"));
        return;
    }

    const auto response = QMessageBox::question(
        this,
        QStringLiteral("Delete Folder"),
        QStringLiteral("Delete this folder? Contained requests will move to Root.")
    );
    if (response != QMessageBox::Yes) {
        return;
    }

    if (!m_requestRepository->deleteFolder(folderId)) {
        statusBar()->showMessage(QStringLiteral("Delete folder failed: %1").arg(m_requestRepository->lastError()));
        return;
    }

    loadSavedData();
    clearCurrentRequest();
    statusBar()->showMessage(QStringLiteral("Folder deleted; requests moved to Root"));
}

void MainWindow::createRequestInSelectedFolder()
{
    const int folderId = selectedFolderId();
    clearCurrentRequest();
    selectFolderInEditor(folderId);
}

void MainWindow::saveCurrentRequest()
{
    SavedRequest request;
    request.id = selectedRequestId();
    request.folderId = currentEditorFolderId();
    request.name = m_requestNameLineEdit->text().trimmed();
    request.method = m_methodComboBox->currentText();
    request.url = m_urlLineEdit->text().trimmed();

    if (request.name.isEmpty() || request.url.isEmpty()) {
        statusBar()->showMessage(QStringLiteral("Request name and URL are required"));
        return;
    }

    const int savedId = request.id >= 0
        ? (m_requestRepository->update(request) ? request.id : -1)
        : m_requestRepository->save(request);

    if (savedId < 0) {
        statusBar()->showMessage(QStringLiteral("Save failed: %1").arg(m_requestRepository->lastError()));
        return;
    }

    loadSavedData();
    selectSavedRequest(savedId);
    statusBar()->showMessage(QStringLiteral("Request saved"));
}

void MainWindow::deleteSelectedRequest()
{
    const int id = selectedRequestId();
    if (id < 0) {
        statusBar()->showMessage(QStringLiteral("Select a request to delete"));
        return;
    }

    if (!m_requestRepository->deleteById(id)) {
        statusBar()->showMessage(QStringLiteral("Delete failed: %1").arg(m_requestRepository->lastError()));
        return;
    }

    loadSavedData();
    clearCurrentRequest();
    statusBar()->showMessage(QStringLiteral("Request deleted"));
}

void MainWindow::clearCurrentRequest()
{
    m_savedRequestsTree->clearSelection();
    m_savedRequestsTree->setCurrentItem(nullptr);
    m_requestNameLineEdit->setText(QStringLiteral("Untitled Request"));
    m_methodComboBox->setCurrentText(QStringLiteral("GET"));
    selectFolderInEditor(-1);
    m_urlLineEdit->setText(QStringLiteral("https://httpbin.org/get"));
    m_responseStatusLabel->setText(QStringLiteral("No response yet"));
    m_responseBodyViewer->clear();
}

void MainWindow::sendCurrentRequest()
{
    const QUrl url = QUrl::fromUserInput(m_urlLineEdit->text().trimmed());
    if (!url.isValid() || url.scheme().isEmpty()) {
        m_responseStatusLabel->setText(QStringLiteral("Invalid URL"));
        m_responseBodyViewer->setPlainText(QStringLiteral("Enter a valid URL, including http:// or https://."));
        return;
    }

    Request request;
    request.method = m_methodComboBox->currentText();
    request.url = url;
    m_httpService->sendGet(request);
}

void MainWindow::setRequestInProgress(bool inProgress)
{
    m_sendButton->setEnabled(!inProgress);
    m_urlLineEdit->setEnabled(!inProgress);
    m_methodComboBox->setEnabled(!inProgress);
    m_folderComboBox->setEnabled(!inProgress);
}

int MainWindow::selectedRequestId() const
{
    const auto *item = m_savedRequestsTree->currentItem();
    if (item == nullptr || item->data(0, Qt::UserRole).toInt() != 1) {
        return -1;
    }

    return item->data(0, Qt::UserRole + 1).toInt();
}

int MainWindow::selectedFolderId() const
{
    const auto *item = m_savedRequestsTree->currentItem();
    if (item == nullptr) {
        return -1;
    }

    if (item->data(0, Qt::UserRole).toInt() == 0) {
        return item->data(0, Qt::UserRole + 1).toInt();
    }

    return item->data(0, Qt::UserRole + 2).toInt();
}

int MainWindow::currentEditorFolderId() const
{
    if (m_folderComboBox->currentIndex() < 0) {
        return -1;
    }

    return m_folderComboBox->currentData().toInt();
}

void MainWindow::selectSavedRequest(int id)
{
    if (id < 0) {
        return;
    }

    auto *item = findRequestItem(id);
    if (item != nullptr) {
        m_savedRequestsTree->setCurrentItem(item);
    }
}

void MainWindow::selectFolderInEditor(int id)
{
    const int index = m_folderComboBox->findData(id);
    m_folderComboBox->setCurrentIndex(index >= 0 ? index : 0);
}

QTreeWidgetItem *MainWindow::findRequestItem(int id) const
{
    const auto items = m_savedRequestsTree->findItems(QStringLiteral("*"), Qt::MatchWildcard | Qt::MatchRecursive);
    for (QTreeWidgetItem *item : items) {
        if (item->data(0, Qt::UserRole).toInt() == 1
            && item->data(0, Qt::UserRole + 1).toInt() == id) {
            return item;
        }
    }

    return nullptr;
}

QTreeWidgetItem *MainWindow::findFolderItem(int id) const
{
    const auto items = m_savedRequestsTree->findItems(QStringLiteral("*"), Qt::MatchWildcard | Qt::MatchRecursive);
    for (QTreeWidgetItem *item : items) {
        if (item->data(0, Qt::UserRole).toInt() == 0
            && item->data(0, Qt::UserRole + 1).toInt() == id) {
            return item;
        }
    }

    return nullptr;
}
