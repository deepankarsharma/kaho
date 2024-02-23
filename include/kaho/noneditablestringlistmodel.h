#pragma once
#include <QStringListModel>
#include <QFlags>


namespace kaho {
    class NonEditableStringListModel : public QStringListModel
    {
    public:
        NonEditableStringListModel(QObject *parent = nullptr);
        Qt::ItemFlags flags(const QModelIndex &index) const override;
    };

} // namespace kaho


