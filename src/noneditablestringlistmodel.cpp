#include <kaho/noneditablestringlistmodel.h>

namespace kaho {
    NonEditableStringListModel::NonEditableStringListModel(QObject *parent) : QStringListModel(parent) {}

    Qt::ItemFlags NonEditableStringListModel::flags(const QModelIndex &index) const {
        return QStringListModel::flags(index) & ~Qt::ItemIsEditable;
    }
} // namespace kaho