#pragma once

#include <QMainWindow>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QNetworkAccessManager>
#include <QSettings>
#include <kaho/localmodelregistry.h>
#include <kaho/server.h>

namespace kaho {
    class MainWindow : public QMainWindow
    {
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        void make_request(const QString& prompt);
        bool is_initialized() const;
        void mark_initialized();

    private:
        QHBoxLayout* m_layout;
        QToolBar* m_toolbar;
        QSettings m_settings;
        LocalModelRegistry m_model_registry;
        Server m_server;
    };

} // namespace kaho

