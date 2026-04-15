#include "ui/SidebarWidget.h"

#include "common/StringUtils.h"

#include <QHash>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QSize>
#include <QSignalBlocker>
#include <QTreeWidget>
#include <QVBoxLayout>

SidebarWidget::SidebarWidget(QWidget *parent)
    : QFrame(parent)
{
    buildLayout();
    connectSignals();
}

void SidebarWidget::setItems(const QVector<Folder> &folders, const QVector<SavedRequest> &requests)
{
    const QSignalBlocker blocker(m_savedRequestsTree);
    m_savedRequestsTree->clear();

    auto *rootItem = new QTreeWidgetItem(m_savedRequestsTree, {QStringLiteral("Root")});
    rootItem->setIcon(0, QIcon(QStringLiteral(":/icons/folder.svg")));
    rootItem->setData(0, Qt::UserRole, FolderItem);
    rootItem->setData(0, Qt::UserRole + 1, -1);
    rootItem->setExpanded(true);

    QHash<int, QTreeWidgetItem *> folderItems;
    for (const Folder &folder : folders) {
        auto *folderItem = new QTreeWidgetItem(m_savedRequestsTree, {folder.name});
        folderItem->setIcon(0, QIcon(QStringLiteral(":/icons/folder.svg")));
        folderItem->setData(0, Qt::UserRole, FolderItem);
        folderItem->setData(0, Qt::UserRole + 1, folder.id);
        folderItem->setExpanded(true);
        folderItems.insert(folder.id, folderItem);
    }

    for (const SavedRequest &request : requests) {
        auto *item = new QTreeWidgetItem({StringUtils::requestDisplayText(request)});
        item->setIcon(0, QIcon(QStringLiteral(":/icons/request.svg")));
        item->setData(0, Qt::UserRole, RequestItem);
        item->setData(0, Qt::UserRole + 1, request.id);
        item->setData(0, Qt::UserRole + 2, request.folderId);
        item->setData(0, Qt::UserRole + 3, request.name);
        item->setData(0, Qt::UserRole + 4, request.method);
        item->setData(0, Qt::UserRole + 5, request.url);

        auto *parentItem = folderItems.value(request.folderId, rootItem);
        parentItem->addChild(item);
    }
}

int SidebarWidget::selectedRequestId() const
{
    const auto *item = m_savedRequestsTree->currentItem();
    if (item == nullptr || item->data(0, Qt::UserRole).toInt() != RequestItem) {
        return -1;
    }

    return item->data(0, Qt::UserRole + 1).toInt();
}

int SidebarWidget::selectedFolderId() const
{
    const auto *item = m_savedRequestsTree->currentItem();
    if (item == nullptr) {
        return -1;
    }

    if (item->data(0, Qt::UserRole).toInt() == FolderItem) {
        return item->data(0, Qt::UserRole + 1).toInt();
    }

    return item->data(0, Qt::UserRole + 2).toInt();
}

QString SidebarWidget::selectedFolderName() const
{
    const auto *item = m_savedRequestsTree->currentItem();
    if (item == nullptr || item->data(0, Qt::UserRole).toInt() != FolderItem) {
        return QString();
    }

    return item->text(0);
}

void SidebarWidget::selectRequest(int id)
{
    if (id < 0) {
        return;
    }

    auto *item = findRequestItem(id);
    if (item != nullptr) {
        m_savedRequestsTree->setCurrentItem(item);
    }
}

void SidebarWidget::selectFolder(int id)
{
    auto *item = findFolderItem(id);
    if (item != nullptr) {
        m_savedRequestsTree->setCurrentItem(item);
    }
}

void SidebarWidget::clearSelection()
{
    m_savedRequestsTree->clearSelection();
    m_savedRequestsTree->setCurrentItem(nullptr);
}

void SidebarWidget::buildLayout()
{
    setProperty("role", "sidebar");
    setMinimumWidth(240);
    setMaximumWidth(360);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    auto *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(8);

    auto *title = new QLabel(QStringLiteral("Saved Requests"), this);
    title->setProperty("role", "title");

    m_addRequestButton = new QPushButton(QStringLiteral("Add Request"), this);
    m_addRequestButton->setIcon(QIcon(QStringLiteral(":/icons/add.svg")));
    m_addRequestButton->setIconSize(QSize(16, 16));
    m_addRequestButton->setProperty("variant", "primary");
    m_addRequestButton->setFixedHeight(32);

    headerLayout->addWidget(title, 1);
    headerLayout->addWidget(m_addRequestButton);

    auto *hint = new QLabel(QStringLiteral("Select a folder or request."), this);
    hint->setProperty("role", "muted");

    m_savedRequestsTree = new QTreeWidget(this);
    m_savedRequestsTree->setFrameShape(QFrame::NoFrame);
    m_savedRequestsTree->setHeaderHidden(true);

    auto *folderActions = new QHBoxLayout();
    folderActions->setSpacing(8);

    m_createFolderButton = new QPushButton(QStringLiteral("Folder"), this);
    m_createFolderButton->setIcon(QIcon(QStringLiteral(":/icons/folder.svg")));
    m_createFolderButton->setIconSize(QSize(16, 16));
    m_renameFolderButton = new QPushButton(QStringLiteral("Rename"), this);
    m_renameFolderButton->setIcon(QIcon(QStringLiteral(":/icons/edit.svg")));
    m_renameFolderButton->setIconSize(QSize(16, 16));
    m_deleteFolderButton = new QPushButton(QStringLiteral("Delete Folder"), this);
    m_deleteFolderButton->setIcon(QIcon(QStringLiteral(":/icons/trash.svg")));
    m_deleteFolderButton->setIconSize(QSize(16, 16));
    m_deleteFolderButton->setProperty("variant", "danger");

    folderActions->addWidget(m_createFolderButton);
    folderActions->addWidget(m_renameFolderButton);

    m_deleteRequestButton = new QPushButton(QStringLiteral("Delete Request"), this);
    m_deleteRequestButton->setIcon(QIcon(QStringLiteral(":/icons/trash.svg")));
    m_deleteRequestButton->setIconSize(QSize(16, 16));
    m_deleteRequestButton->setProperty("variant", "danger");

    layout->addLayout(headerLayout);
    layout->addWidget(hint);
    layout->addWidget(m_savedRequestsTree, 1);
    layout->addLayout(folderActions);
    layout->addWidget(m_deleteFolderButton);
    layout->addWidget(m_deleteRequestButton);
}

void SidebarWidget::connectSignals()
{
    connect(m_addRequestButton, &QPushButton::clicked, this, &SidebarWidget::addRequestRequested);
    connect(m_createFolderButton, &QPushButton::clicked, this, &SidebarWidget::createFolderRequested);
    connect(m_renameFolderButton, &QPushButton::clicked, this, &SidebarWidget::renameFolderRequested);
    connect(m_deleteFolderButton, &QPushButton::clicked, this, &SidebarWidget::deleteFolderRequested);
    connect(m_deleteRequestButton, &QPushButton::clicked, this, &SidebarWidget::deleteRequestRequested);

    connect(m_savedRequestsTree, &QTreeWidget::currentItemChanged, this, [this](QTreeWidgetItem *current) {
        if (current == nullptr) {
            return;
        }

        if (current->data(0, Qt::UserRole).toInt() == RequestItem) {
            emit requestSelected(requestFromItem(current));
        } else {
            emit folderSelected(current->data(0, Qt::UserRole + 1).toInt());
        }
    });
}

SavedRequest SidebarWidget::requestFromItem(const QTreeWidgetItem *item) const
{
    SavedRequest request;
    request.id = item->data(0, Qt::UserRole + 1).toInt();
    request.folderId = item->data(0, Qt::UserRole + 2).toInt();
    request.name = item->data(0, Qt::UserRole + 3).toString();
    request.method = item->data(0, Qt::UserRole + 4).toString();
    request.url = item->data(0, Qt::UserRole + 5).toString();
    return request;
}

QTreeWidgetItem *SidebarWidget::findRequestItem(int id) const
{
    const auto items = m_savedRequestsTree->findItems(QStringLiteral("*"), Qt::MatchWildcard | Qt::MatchRecursive);
    for (QTreeWidgetItem *item : items) {
        if (item->data(0, Qt::UserRole).toInt() == RequestItem
            && item->data(0, Qt::UserRole + 1).toInt() == id) {
            return item;
        }
    }

    return nullptr;
}

QTreeWidgetItem *SidebarWidget::findFolderItem(int id) const
{
    const auto items = m_savedRequestsTree->findItems(QStringLiteral("*"), Qt::MatchWildcard | Qt::MatchRecursive);
    for (QTreeWidgetItem *item : items) {
        if (item->data(0, Qt::UserRole).toInt() == FolderItem
            && item->data(0, Qt::UserRole + 1).toInt() == id) {
            return item;
        }
    }

    return nullptr;
}
