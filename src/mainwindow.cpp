#include <kaho/mainwindow.h>
#include <QToolButton>
#include <QStyle>
#include <QStackedWidget>
#include <kaho/chatview.h>
#include <kaho/aimodel.h>




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
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

    auto add_button = [this](const QString& title, int index, const QString& path) {
        QToolButton *toolButton = new QToolButton();
        toolButton->setText(title);

        QIcon icon(path);
        toolButton->setIcon(icon);
        connect(toolButton, &QToolButton::clicked, [this, index] {
            auto cwidget = this->centralWidget();
            auto stacked = qobject_cast<QStackedWidget*>(cwidget);
            stacked->setCurrentIndex(index);
        });
        this->m_toolbar->addWidget(toolButton);
        return;
    };

    add_button("button_1", 0, ":/images/icons8-chat.svg");

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_toolbar->addWidget(spacer);
    add_button("button_4", 3, ":/images/icons8-settings.svg");

}

MainWindow::~MainWindow() {}



