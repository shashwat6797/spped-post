#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class HttpService;
class FolderRepository;
class FolderService;
class MainWindowController;
class RequestRepository;
class RequestService;
class RequestWorkspaceWidget;
class SidebarWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private:
    void configureWindow();
    void assembleUi();
    void createProviders();
    void startController();

    HttpService *m_httpService = nullptr;
    FolderRepository *m_folderRepository = nullptr;
    RequestRepository *m_requestRepository = nullptr;
    FolderService *m_folderService = nullptr;
    RequestService *m_requestService = nullptr;
    SidebarWidget *m_sidebar = nullptr;
    RequestWorkspaceWidget *m_workspace = nullptr;
    MainWindowController *m_controller = nullptr;
};

#endif // MAINWINDOW_H
