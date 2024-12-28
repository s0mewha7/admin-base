#include "database.h"
#include <QDebug>
#include <QRegularExpression>

DataModel::DataModel(QObject* parent)
        : QAbstractTableModel(parent) {}

int DataModel::rowCount(const QModelIndex&) const {
    return m_data.size();
}

int DataModel::columnCount(const QModelIndex&) const {
    return m_headers.size();
}

QVariant DataModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_data.size() || index.column() >= m_headers.size())
        return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const QJsonObject obj = m_data[index.row()].toObject();
        return obj.value(m_headers[index.column()]).toVariant();
    }
    return {};
}

bool DataModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || index.row() >= m_data.size() || index.column() >= m_headers.size() || role != Qt::EditRole)
        return false;

    QString inputValue = value.toString();
    QString currentHeader = m_headers[index.column()];

    QRegularExpression emojiRegex("[\\x{1F600}-\\x{1F64F}"
                                  "|\\x{1F300}-\\x{1F5FF}"
                                  "|\\x{1F680}-\\x{1F6FF}"
                                  "|\\x{1F700}-\\x{1F77F}"
                                  "|\\x{1F780}-\\x{1F7FF}"
                                  "|\\x{1F800}-\\x{1F8FF}"
                                  "|\\x{1F900}-\\x{1F9FF}"
                                  "|\\x{1FA00}-\\x{1FAFF}"
                                  "|\\x{2600}-\\x{26FF}"
                                  "|\\x{2700}-\\x{27BF}]+");

    if (emojiRegex.match(inputValue).hasMatch()) {
        qWarning() << "Emojis are not allowed.";
        return false;
    }

    if (currentHeader == "mac") {
        QRegularExpression macRegex(
                "^(?:[0-9A-Fa-f]{2}[:-]?){5}[0-9A-Fa-f]{2}$|^([0-9A-Fa-f]{12})$|^([0-9A-Fa-f]{2}[ ]?){5}([0-9A-Fa-f]{2})$");
        if (!macRegex.match(inputValue).hasMatch()) {
            qWarning() << "Invalid MAC address format.";
            return false;
        }
    }

    if (currentHeader == "end_of_life_date") {  // Убедитесь, что заголовок настроен правильно
        QRegularExpression dateRegex(R"(^(\d{4}-\d{2}-\d{2})$|^(\d{2}\.\d{2}\.\d{4})$|^(\d{2}/\d{2}/\d{4})$)");

        if (dateRegex.match(inputValue).hasMatch()) {
            QDate date;

            if (inputValue.contains('-')) {
                date = QDate::fromString(inputValue, "yyyy-MM-dd");
            } else if (inputValue.contains('.')) {
                date = QDate::fromString(inputValue, "dd.MM.yyyy");
            } else if (inputValue.contains('/')) {
                date = QDate::fromString(inputValue, "MM/dd/yyyy");
            }

            if (!date.isValid()) {
                qWarning() << "Invalid date:" << inputValue;
                return false;
            }
        } else {
            qWarning() << "Invalid date format for value:" << inputValue;
            return false;
        }
    }


    QJsonObject obj = m_data[index.row()].toObject();
    obj[m_headers[index.column()]] = QJsonValue::fromVariant(value);
    m_data[index.row()] = obj;

    emit dataChanged(index, index);
    saveToFile();
    return true;
}

Qt::ItemFlags DataModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant DataModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return m_headers.value(section);
    }
    return {};
}

bool DataModel::loadFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << filePath;
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isArray()) {
        qWarning() << "Invalid JSON format in file:" << filePath;
        return false;
    }

    QJsonArray newData = doc.array();

    beginResetModel();
    m_data = newData;
    if (!m_data.isEmpty()) {
        const QJsonObject obj = m_data.first().toObject();
        m_headers = obj.keys();
    }
    endResetModel();

    return true;
}

void DataModel::saveToFile() const {
    if (m_filePath.isEmpty())
        return;
    if (m_updatesLocked) {
        qWarning() << "Failed to save file:" << m_filePath;
        return;
    }
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to save file:" << m_filePath;
        return;
    }

    QJsonDocument doc(m_data);
    file.write(doc.toJson());
    file.close();
}

void DataModel::setFilePath(const QString& filePath) {
    m_filePath = filePath;
    loadData();
}

void DataModel::addRow() {
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());

    QJsonObject newRow;
    for (const QString& header : m_headers) {
        newRow[header] = "";
    }
    m_data.append(newRow);

    endInsertRows();
}

void DataModel::removeRow(int row) {
    if (row < 0 || row >= m_data.size()) {
        qWarning() << "Invalid row index for removal:" << row;
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_data.removeAt(row);
    endRemoveRows();

    emit fileChanged();

    if (!m_updatesLockedFile) {
        saveToFile();
    }

    qDebug() << "Row" << row << "deleted. Remaining data:" << m_data;
}

void DataModel::lockUpdates(bool lock) {
    m_updatesLocked = lock;
}

void DataModel::loadData() {
    if (!m_filePath.isEmpty()) {
        loadFromFile(m_filePath);
    }
}
