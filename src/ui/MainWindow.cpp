#include "ui/MainWindow.h"

#include "modules/folder/FolderRepository.h"
#include "modules/folder/FolderService.h"
#include "modules/request/HttpService.h"
#include "modules/request/RequestRepository.h"
#include "modules/request/RequestService.h"
#include "ui/MainWindowController.h"
#include "ui/RequestWorkspaceWidget.h"
#include "ui/SidebarWidget.h"

#include <QSplitter>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    configureWindow();
    createProviders();
    assembleUi();
    startController();
}

void MainWindow::configureWindow()
{
    setWindowTitle(QStringLiteral("SpeedPost"));
    setMinimumSize(1100, 700);
    resize(1280, 800);
}

void MainWindow::createProviders()
{
    m_httpService = new HttpService(this);
    m_folderRepository = new FolderRepository(this);
    m_requestRepository = new RequestRepository(this);
    m_folderService = new FolderService(m_folderRepository, this);
    m_requestService = new RequestService(m_requestRepository, this);
}

void MainWindow::assembleUi()
{
    m_sidebar = new SidebarWidget(this);
    m_workspace = new RequestWorkspaceWidget(this);

    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setChildrenCollapsible(true);
    splitter->addWidget(m_sidebar);
    splitter->addWidget(m_workspace);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({280, 1000});

    setCentralWidget(splitter);
}

void MainWindow::startcontroller()
{
    m_controller = new MainWindowController(
        m_sidebar,
        m_workspace,
        m_folderService,
        m_requestService,
        m_httpService,
        statusBar(),
        this,
        this
    );
    m_controller->start();
}
