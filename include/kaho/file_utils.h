#pragma once

#include <QString>
#include <QFile>
#include <QTextStream>

int to_file(const QString& path, const QByteArray& byteArray);
QString from_file(const QString& path);
