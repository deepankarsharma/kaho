#pragma once

#include <QMainWindow>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QNetworkAccessManager>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void make_request(const QString& prompt);

private:
    QHBoxLayout* m_layout;
    QToolBar* m_toolbar;

};

