#include <kaho/noneditablestringlistmodel.h>

NonEditableStringListModel::NonEditableStringListModel(QObject *parent) : QStringListModel(parent) {}

Qt::ItemFlags NonEditableStringListModel::flags(const QModelIndex &index) const {
    return QStringListModel::flags(index) & ~Qt::ItemIsEditable;
}
