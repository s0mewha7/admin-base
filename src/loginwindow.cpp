#include "loginwindow.h"
#include <QApplication>

LoginWindow::LoginWindow(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void LoginWindow::setupUi() {
    setWindowTitle("Login");
    resize(300, 150);
    auto* layout = new QVBoxLayout(this);
    auto* usernameLayout = new QHBoxLayout();
    auto* usernameLabel = new QLabel("Username:", this);
    m_usernameEdit = new QLineEdit(this);
    usernameLayout->addWidget(usernameLabel);
    usernameLayout->addWidget(m_usernameEdit);

    auto* passwordLayout = new QHBoxLayout();
    auto* passwordLabel = new QLabel("Password:", this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(m_passwordEdit);

    m_loginButton = new QPushButton("Login", this);

    layout->addLayout(usernameLayout);
    layout->addLayout(passwordLayout);
    layout->addWidget(m_loginButton);

    setLayout(layout);

    connect(m_loginButton, &QPushButton::clicked, this, &LoginWindow::attemptLogin);
}

void LoginWindow::attemptLogin() {
    const QString username = m_usernameEdit->text().trimmed();
    const QString password = m_passwordEdit->text().trimmed();

    if (username == "admin" && password == "admin") {
        auto* mainWindow = new MainWindow();
        mainWindow->resize(800, 600);
        mainWindow->show();

        close();
    } else {
        QMessageBox::warning(this, "Error", "Invalid username or password.");
    }
}