#include "ui/RequestWorkspaceWidget.h"

#include <QComboBox>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <QUrl>
#include <QVBoxLayout>

RequestWorkspaceWidget::RequestWorkspaceWidget(QWidget *parent)
    : QWidget(parent)
{
    buildLayout();
    connectSignals();
}

void RequestWorkspaceWidget::setFolders(const QVector<Folder> &folders)
{
    const int folderId = currentFolderId();
    m_folderComboBox->clear();
    m_folderComboBox->addItem(QStringLiteral("Root"), -1);

    for (const Folder &folder : folders) {
        m_folderComboBox->addItem(folder.name, folder.id);
    }

    setCurrentFolderId(folderId);
}

int RequestWorkspaceWidget::currentFolderId() const
{
    if (m_folderComboBox->currentIndex() < 0) {
        return -1;
    }

    return m_folderComboBox->currentData().toInt();
}

void RequestWorkspaceWidget::setCurrentFolderId(int id)
{
    const int index = m_folderComboBox->findData(id);
    m_folderComboBox->setCurrentIndex(index >= 0 ? index : 0);
}

SavedRequest RequestWorkspaceWidget::currentDraft(int id) const
{
    SavedRequest request;
    request.id = id;
    request.folderId = currentFolderId();
    request.name = m_requestNameLineEdit->text();
    request.method = m_methodComboBox->currentText();
    request.url = m_urlLineEdit->text();
    return request;
}

void RequestWorkspaceWidget::loadRequest(const SavedRequest &request)
{
    m_requestNameLineEdit->setText(request.name);
    m_methodComboBox->setCurrentText(request.method);
    m_urlLineEdit->setText(request.url);
    setCurrentFolderId(request.folderId);
    showLoadedRequest();
}

void RequestWorkspaceWidget::clearRequest(int folderId)
{
    m_requestNameLineEdit->setText(QStringLiteral("Untitled Request"));
    m_methodComboBox->setCurrentText(QStringLiteral("GET"));
    setCurrentFolderId(folderId);
    m_urlLineEdit->setText(QStringLiteral("https://httpbin.org/get"));
    m_responseStatusLabel->setText(QStringLiteral("No response yet"));
    m_responseBodyViewer->clear();
}

void RequestWorkspaceWidget::setRequestInProgress(bool inProgress)
{
    m_sendButton->setEnabled(!inProgress);
    m_urlLineEdit->setEnabled(!inProgress);
    m_methodComboBox->setEnabled(!inProgress);
    m_folderComboBox->setEnabled(!inProgress);
}

void RequestWorkspaceWidget::showLoadedRequest()
{
    m_responseStatusLabel->setText(QStringLiteral("Loaded saved request"));
    emit statusMessageChanged(QStringLiteral("Loaded saved request"));
}

void RequestWorkspaceWidget::showSending()
{
    setRequestInProgress(true);
    m_responseStatusLabel->setText(QStringLiteral("Sending..."));
    m_responseBodyViewer->clear();
    emit statusMessageChanged(QStringLiteral("Sending request"));
}

void RequestWorkspaceWidget::showResponse(const QString &body)
{
    setRequestInProgress(false);
    m_responseStatusLabel->setText(QStringLiteral("Response received"));
    m_responseBodyViewer->setPlainText(body);
    emit statusMessageChanged(QStringLiteral("Request complete"));
}

void RequestWorkspaceWidget::showFailure(const QString &message)
{
    setRequestInProgress(false);
    m_responseStatusLabel->setText(QStringLiteral("Request failed"));
    m_responseBodyViewer->setPlainText(message);
    emit statusMessageChanged(QStringLiteral("Request failed"));
}

void RequestWorkspaceWidget::buildLayout()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(14);

    auto *topBar = new QFrame(this);
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

    auto *requestPanel = new QFrame(this);
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
    auto *bodyEditor = new QPlainTextEdit(tabs);
    bodyEditor->setPlaceholderText(QStringLiteral("{\n  \"name\": \"Ada Lovelace\"\n}"));

    tabs->addTab(createKeyValueTable(tabs), QStringLiteral("Params"));
    tabs->addTab(createKeyValueTable(tabs), QStringLiteral("Headers"));
    tabs->addTab(bodyEditor, QStringLiteral("Body"));

    requestLayout->addLayout(requestLine);
    requestLayout->addWidget(tabs, 1);

    auto *responsePanel = new QFrame(this);
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
}

void RequestWorkspaceWidget::connectSignals()
{
    connect(m_saveRequestButton, &QPushButton::clicked, this, &RequestWorkspaceWidget::saveRequested);
    connect(m_sendButton, &QPushButton::clicked, this, &RequestWorkspaceWidget::requestSend);
    connect(m_urlLineEdit, &QLineEdit::returnPressed, this, &RequestWorkspaceWidget::requestSend);
}

QTableWidget *RequestWorkspaceWidget::createKeyValueTable(QWidget *parent) const
{
    auto *table = new QTableWidget(4, 3, parent);
    table->setHorizontalHeaderLabels({
        QStringLiteral("Key"),
        QStringLiteral("Value"),
        QStringLiteral("Description")
    });
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->setAlternatingRowColors(true);
    return table;
}

void RequestWorkspaceWidget::requestSend()
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
    emit sendRequested(request);
}
