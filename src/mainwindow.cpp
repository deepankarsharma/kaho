#include <kaho/mainwindow.h>
#include <QToolButton>
#include <QStyle>
#include <QStackedWidget>
#include <kaho/chatview.h>
#include <kaho/aimodel.h>
#include <kaho/downloader.h>
#include <kaho/file_utils.h>


namespace kaho {
    MainWindow::MainWindow(QWidget *parent)
            : QMainWindow(parent), m_settings("kaho.ai", "kaho") {

        // TODO: add the following tests
        // 1. file exists but is empty
        // 2. file exists but has wrong checksum
        // 3. file exists is missing checksum
        // 4. file exists and is incomplete
        QString model_url("https://huggingface.co/TheBloke/OpenHermes-2.5-Mistral-7B-GGUF/resolve/main/openhermes-2.5-mistral-7b.Q4_K_M.gguf");
        m_settings.setValue("model_url", model_url);

        m_model_registry.ensureModel(model_url);
        m_server.start(model_url);

        m_toolbar = new QToolBar("toolbar");
        m_toolbar->setIconSize(QSize(48, 48));
        m_toolbar->setMovable(false);
        m_toolbar->setAllowedAreas(Qt::LeftToolBarArea);
        m_toolbar->setOrientation(Qt::Vertical);
        auto cwidget = new QStackedWidget();
        auto chat_view = new ChatView();

        cwidget->addWidget(chat_view);
        setCentralWidget(cwidget);

        this->addToolBar(Qt::LeftToolBarArea, m_toolbar);

        auto add_button = [this](const QString &title, int index, const QString &path) {
            QToolButton *toolButton = new QToolButton();
            toolButton->setText(title);

            QIcon icon(path);
            toolButton->setIcon(icon);
            connect(toolButton, &QToolButton::clicked, [this, index] {
                auto cwidget = this->centralWidget();
                auto stacked = qobject_cast<QStackedWidget *>(cwidget);
                stacked->setCurrentIndex(index);
            });
            this->m_toolbar->addWidget(toolButton);
            return;
        };

        add_button("button_1", 0, ":/images/icons8-chat.svg");

        QWidget *spacer = new QWidget();
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_toolbar->addWidget(spacer);
        add_button("button_4", 3, ":/images/icons8-settings.svg");

    }

    MainWindow::~MainWindow() {
        m_server.stop();
    }

    bool MainWindow::is_initialized() const {
        return m_settings.contains("initialized");
    }

    void MainWindow::mark_initialized() {
        m_settings.setValue("initialized", true);
    }
} // namespace kaho

