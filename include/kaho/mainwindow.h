#pragma once

#include <QMainWindow>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QNetworkAccessManager>
#include <QSettings>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void make_request(const QString& prompt);
    bool is_first_run() const;
    bool mark_first_run();

private:
    QHBoxLayout* m_layout;
    QToolBar* m_toolbar;
    QSettings m_settings;
};

