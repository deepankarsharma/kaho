#include <kaho/file_utils.h>


//int to_file(const QString& path, const QByteArray& byteArray) {

//    QFile file(path);

//    if (file.open(QIODevice::WriteOnly)) {
//        qint64 bytesWritten = file.write(byteArray);

//        if (bytesWritten == -1) {
//            // Handle the error if the write operation fails.
//            return 1;
//        }

//        file.close();
//        return 0; // Successfully wrote the QByteArray to the file.
//    } else {
//        // Handle the error if the file couldn't be opened.
//        return 1;
//    }
//}


int to_file(const QString& path, const QByteArray& byteArray) {
    QFile file(path); // Replace with your desired output file path
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Handle the error if the file couldn't be opened
        return 1;
    }

    // Convert QByteArray to QString
    QString textContent = QString::fromUtf8(byteArray);

    // Write the QString to the file
    QTextStream out(&file);
    out << textContent;

    file.close();

    return 0; // Successful execution
}

QString from_file(const QString& path) {
    // test ...
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Handle the error if the file couldn't be opened
        return "<ERROR>";
    }

    QTextStream in(&file);
    QString content = in.readAll();

    file.close();
    return content;
}
