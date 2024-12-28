#pragma once

#include <QMainWindow>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileSystemWatcher>
#include "database.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QTableView* m_tableView;
    DataModel* m_model;
    QPushButton* m_addRowButton;
    QPushButton* m_saveButton;
    QPushButton* m_deleteButton;
    QPushButton* m_backupButton;
    QFileSystemWatcher* m_watcher;

    void setupUi();
    void setupFileWatcher(const QString& filePath);
    void deleteSelectedRow();
};