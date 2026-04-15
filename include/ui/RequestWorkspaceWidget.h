#ifndef REQUESTWORKSPACEWIDGET_H
#define REQUESTWORKSPACEWIDGET_H

#include "modules/folder/FolderModel.h"
#include "modules/request/RequestModel.h"
#include "modules/request/SavedRequestModel.h"

#include <QWidget>
#include <QVector>

class QLabel;
class QComboBox;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QTableWidget;

class RequestWorkspaceWidget : public QWidget {
    Q_OBJECT

public:
    explicit RequestWorkspaceWidget(QWidget *parent = nullptr);

    void setFolders(const QVector<Folder> &folders);
    int currentFolderId() const;
    void setCurrentFolderId(int id);
    SavedRequest currentDraft(int id) const;
    void loadRequest(const SavedRequest &request);
    void clearRequest(int folderId = -1);
    void setRequestInProgress(bool inProgress);
    void showLoadedRequest();
    void showSending();
    void showResponse(const QString &body);
    void showFailure(const QString &message);

signals:
    void saveRequested();
    void sendRequested(const Request &request);
    void statusMessageChanged(const QString &message);

private:
    void buildLayout();
    void connectSignals();
    QTableWidget *createKeyValueTable(QWidget *parent) const;
    void requestSend();

    QLineEdit *m_requestNameLineEdit = nullptr;
    QComboBox *m_methodComboBox = nullptr;
    QComboBox *m_folderComboBox = nullptr;
    QLineEdit *m_urlLineEdit = nullptr;
    QPushButton *m_saveRequestButton = nullptr;
    QPushButton *m_sendButton = nullptr;
    QLabel *m_responseStatusLabel = nullptr;
    QPlainTextEdit *m_responseBodyViewer = nullptr;
};

#endif // REQUESTWORKSPACEWIDGET_H
