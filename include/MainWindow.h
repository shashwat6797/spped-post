#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class HttpService;
class RequestRepository;
class QLabel;
class QComboBox;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private:
    QWidget *createSidebar();
    QWidget *createRequestWorkspace();
    void connectRequestFlow();
    void connectPersistenceFlow();
    void loadSavedData();
    void populateRequestInputs(int id);
    void populateFolderSelector();
    void createFolder();
    void renameSelectedFolder();
    void deleteSelectedFolder();
    void createRequestInSelectedFolder();
    void saveCurrentRequest();
    void deleteSelectedRequest();
    void clearCurrentRequest();
    void sendCurrentRequest();
    void setRequestInProgress(bool inProgress);
    int selectedRequestId() const;
    int selectedFolderId() const;
    int currentEditorFolderId() const;
    void selectSavedRequest(int id);
    void selectFolderInEditor(int id);
    QTreeWidgetItem *findRequestItem(int id) const;
    QTreeWidgetItem *findFolderItem(int id) const;

    HttpService *m_httpService = nullptr;
    RequestRepository *m_requestRepository = nullptr;
    QTreeWidget *m_savedRequestsTree = nullptr;
    QLineEdit *m_requestNameLineEdit = nullptr;
    QComboBox *m_methodComboBox = nullptr;
    QComboBox *m_folderComboBox = nullptr;
    QLineEdit *m_urlLineEdit = nullptr;
    QPushButton *m_addRequestButton = nullptr;
    QPushButton *m_createFolderButton = nullptr;
    QPushButton *m_renameFolderButton = nullptr;
    QPushButton *m_deleteFolderButton = nullptr;
    QPushButton *m_saveRequestButton = nullptr;
    QPushButton *m_deleteRequestButton = nullptr;
    QPushButton *m_sendButton = nullptr;
    QLabel *m_responseStatusLabel = nullptr;
    QPlainTextEdit *m_responseBodyViewer = nullptr;
};

#endif // MAINWINDOW_H
