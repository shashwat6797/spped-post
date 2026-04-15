#ifndef MAINWINDOWCONTROLLER_H
#define MAINWINDOWCONTROLLER_H

#include <QObject>

class FolderService;
class HttpService;
class QStatusBar;
class RequestService;
class RequestWorkspaceWidget;
class SidebarWidget;
class QWidget;

class MainWindowController : public QObject {
    Q_OBJECT

public:
    MainWindowController(
        SidebarWidget *sidebar,
        RequestWorkspaceWidget *workspace,
        FolderService *folderService,
        RequestService *requestService,
        HttpService *httpService,
        QStatusBar *statusBar,
        QWidget *dialogParent,
        QObject *parent = nullptr
    );

    void start();

private:
    void connectRequestFlow();
    void connectPersistenceFlow();
    void loadSavedData();
    void createFolder();
    void renameSelectedFolder();
    void deleteSelectedFolder();
    void createRequestInSelectedFolder();
    void saveCurrentRequest();
    void deleteSelectedRequest();
    void showStatus(const QString &message);

    SidebarWidget *m_sidebar = nullptr;
    RequestWorkspaceWidget *m_workspace = nullptr;
    FolderService *m_folderService = nullptr;
    RequestService *m_requestService = nullptr;
    HttpService *m_httpService = nullptr;
    QStatusBar *m_statusBar = nullptr;
    QWidget *m_dialogParent = nullptr;
};

#endif // MAINWINDOWCONTROLLER_H
