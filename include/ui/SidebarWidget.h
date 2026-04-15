#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include "modules/folder/FolderModel.h"
#include "modules/request/SavedRequestModel.h"

#include <QFrame>
#include <QVector>

class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

class SidebarWidget : public QFrame {
    Q_OBJECT

public:
    explicit SidebarWidget(QWidget *parent = nullptr);

    void setItems(const QVector<Folder> &folders, const QVector<SavedRequest> &requests);
    int selectedRequestId() const;
    int selectedFolderId() const;
    QString selectedFolderName() const;
    void selectRequest(int id);
    void selectFolder(int id);
    void clearSelection();

signals:
    void addRequestRequested();
    void createFolderRequested();
    void renameFolderRequested();
    void deleteFolderRequested();
    void deleteRequestRequested();
    void folderSelected(int id);
    void requestSelected(const SavedRequest &request);

private:
    enum ItemType {
        FolderItem = 0,
        RequestItem = 1
    };

    void buildLayout();
    void connectSignals();
    SavedRequest requestFromItem(const QTreeWidgetItem *item) const;
    QTreeWidgetItem *findRequestItem(int id) const;
    QTreeWidgetItem *findFolderItem(int id) const;

    QTreeWidget *m_savedRequestsTree = nullptr;
    QPushButton *m_addRequestButton = nullptr;
    QPushButton *m_createFolderButton = nullptr;
    QPushButton *m_renameFolderButton = nullptr;
    QPushButton *m_deleteFolderButton = nullptr;
    QPushButton *m_deleteRequestButton = nullptr;
};

#endif // SIDEBARWIDGET_H
