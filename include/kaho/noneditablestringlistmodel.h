#pragma once
#include <QStringListModel>
#include <QFlags>

class NonEditableStringListModel : public QStringListModel
{
public:
    NonEditableStringListModel(QObject *parent = nullptr);
    Qt::ItemFlags flags(const QModelIndex &index) const override;
};


