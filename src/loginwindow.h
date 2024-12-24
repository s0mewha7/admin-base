#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include "mainwindow.h"

class LoginWindow : public QWidget {
Q_OBJECT

public:
    explicit LoginWindow(QWidget* parent = nullptr);

private slots:
    void attemptLogin();

private:
    QLineEdit* m_usernameEdit;
    QLineEdit* m_passwordEdit;
    QPushButton* m_loginButton;

    void setupUi();
};