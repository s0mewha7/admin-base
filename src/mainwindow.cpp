#include "mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget* parent)
        : QMainWindow(parent), m_tableView(new QTableView(this)), m_model(new DataModel(this)), m_watcher(new QFileSystemWatcher(this)) {
    setupUi();

    const QString jsonFilePath = "/home/somewhat/CLionProjects/admin-base/data /data.json";
    m_model->setFilePath(jsonFilePath);
    setupFileWatcher(jsonFilePath);

    m_tableView->setModel(m_model);

    connect(m_addRowButton, &QPushButton::clicked, m_model, &DataModel::addRow);
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedRow);
    connect(m_saveButton, &QPushButton::clicked, m_model, &DataModel::saveToFile);
}

void MainWindow::setupFileWatcher(const QString& filePath) {
    if (!m_watcher->addPath(filePath)) {
        qWarning() << "Failed to watch the file:" << filePath;
        return;
    }

    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, [this, filePath]() {
        qDebug() << "File changed. Reloading data.";
        m_model->lockUpdates(true);
        m_model->loadFromFile(filePath);
        m_model->lockUpdates(false);
        m_watcher->addPath(filePath);
    });
}

void MainWindow::deleteSelectedRow() {
    QModelIndexList selectedRows = m_tableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No row selected for deletion.");
        return;
    }

    int selectedRow = selectedRows.first().row();
    m_model->removeRow(selectedRow);
}

void MainWindow::setupUi() {
    auto* centralWidget = new QWidget(this);
    auto* layout = new QVBoxLayout(centralWidget);

    m_addRowButton = new QPushButton("Add", this);
    m_saveButton = new QPushButton("Save", this);
    m_deleteButton = new QPushButton("Delete", this);
    m_backupButton = new QPushButton("Backup", this);

    layout->addWidget(m_tableView);
    layout->addWidget(m_addRowButton);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_saveButton);
    layout->addWidget(m_backupButton);

    setCentralWidget(centralWidget);

    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
}