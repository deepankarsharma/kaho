#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QStringListModel>
#include <QLineEdit>
#include <QPushButton>
#include <QListView>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTranslator>
#include <QObject>
#include <QCoreApplication>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QFile>
#include <kaho/chatview.h>
#include <kaho/promptedit.h>
#include <kaho/aimodel.h>
#include <kaho/noneditablestringlistmodel.h>
#include <kaho/downloader.h>

namespace kaho {
    void readJson() {
        QString val;
        QFile file;
        file.setFileName("test.json");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        val = file.readAll();
        file.close();
        qWarning() << val;
        QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject sett2 = d.object();
        QJsonValue value = sett2.value(QString("appName"));
        qWarning() << value;
        QJsonObject item = value.toObject();

        /* in case of string value get value and convert into string*/
        QJsonValue subobj = item["description"];
        qWarning() << subobj.toString();

        /* in case of array get array and convert into string*/
        QJsonArray test = item["imp"].toArray();
        qWarning() << test[1].toString();
    }

    ChatView::ChatView(QWidget *parent) : QWidget(parent) {
        initializeUi();
    }


    void ChatView::initializeUi() {
        // ┌───────────────────────────────────────────────────────────────────────────────────────────┐
        // │                                                                                           │
        // │  ┌─────────────────────────────────────────────┐  ┌─────────────────────────────────────┐ │
        // │  │  firstColumnLayout                          │  │  secondColumnLayout                 │ │
        // │  │                                             │  │ ┌────────────────────┐              │ │
        // │  │ ┌─────────────────────────────────────────┐ │  │ │questionLabel       │              │ │
        // │  │ │ m_view_questions                        │ │  │ │                    │              │ │
        // │  │ │                                         │ │  │ └────────────────────┘              │ │
        // │  │ │                                         │ │  │                                     │ │
        // │  │ │                                         │ │  │ ┌─────────────────────────────────┐ │ │
        // │  │ │                                         │ │  │ │m_view_current_question          │ │ │
        // │  │ │                                         │ │  │ │                                 │ │ │
        // │  │ │                                         │ │  │ │                                 │ │ │
        // │  │ │                                         │ │  │ │                                 │ │ │
        // │  │ │                                         │ │  │ │                                 │ │ │
        // │  │ │                                         │ │  │ │                                 │ │ │
        // │  │ │                                         │ │  │ └─────────────────────────────────┘ │ │
        // │  │ │                                         │ │  │                                     │ │
        // │  │ │                                         │ │  │ ┌─────────────────────┐             │ │
        // │  │ │                                         │ │  │ │answerLabel          │             │ │
        // │  │ │                                         │ │  │ │                     │             │ │
        // │  │ │                                         │ │  │ └─────────────────────┘             │ │
        // │  │ │                                         │ │  │                                     │ │
        // │  │ │                                         │ │  │ ┌─────────────────────────────────┐ │ │
        // │  │ │                                         │ │  │ │m_view_current_answer            │ │ │
        // │  │ │                                         │ │  │ │                                 │ │ │
        // │  │ │                                         │ │  │ │                                 │ │ │
        // │  │ │                                         │ │  │ └─────────────────────────────────┘ │ │
        // │  │ │                                         │ │  │                                     │ │
        // │  │ │                                         │ │  │ ┌─────────────────────────────────┐ │ │
        // │  │ │                                         │ │  │ │m_view_prompt                    │ │ │
        // │  │ │                                         │ │  │ │                                 │ │ │
        // │  │ └─────────────────────────────────────────┘ │  │ └─────────────────────────────────┘ │ │
        // │  │                                             │  │                                     │ │
        // │  └─────────────────────────────────────────────┘  └─────────────────────────────────────┘ │
        // │                                                                                           │
        // └───────────────────────────────────────────────────────────────────────────────────────────┘

        auto mainLayout = new QHBoxLayout(this);

        auto firstColumnVLayout = new QVBoxLayout();

        m_view_questions = new QListView();
        auto model = new NonEditableStringListModel();
        m_view_questions->setModel(model);
        firstColumnVLayout->addWidget(m_view_questions);

        mainLayout->addLayout(firstColumnVLayout, 1);


        auto secondColumnVLayout = new QVBoxLayout();

        m_progress_bar = new QProgressBar();
        m_progress_bar->setRange(0, 0);

        m_view_current_answer = new QTextEdit();
        secondColumnVLayout->addWidget(m_view_current_answer, 5);

        auto prompt = new PromptEdit();
        m_view_prompt = prompt;
        secondColumnVLayout->addWidget(prompt, 1);


        auto aiModel = new AIModel();

        QObject::connect(prompt, &PromptEdit::promptEntered, this, &ChatView::promptEnteredUpdateUi);
        QObject::connect(this, &ChatView::promptEnteredUpdateModels, aiModel, &AIModel::processPrompt);

        QObject::connect(aiModel, &AIModel::answerFragmentReceived, [this](const QString &response) {
            //qDebug() << "AI Model Response <>: " << response << " response.length: " << response.length();
            auto first_brace_index = response.indexOf('{');
            if (first_brace_index >= 1) {
                auto rest = response.mid(first_brace_index - 1);
                QJsonDocument jsonDoc = QJsonDocument::fromJson(rest.toUtf8());
                auto content = jsonDoc["choices"][0]["delta"]["content"];
                m_answer += content.toString();
                this->m_view_current_answer->setMarkdown(this->m_answer);
            }
        });

        mainLayout->addLayout(secondColumnVLayout, 3);

        setLayout(mainLayout);
    }

    void ChatView::promptEnteredUpdateUi(const QString &prompt) {
        m_view_prompt->setText("");
        m_view_current_answer->setText("");
        m_answer = "";
        auto m = m_view_questions->model();
        if (m->insertRow(m->rowCount())) {
            QModelIndex index = m->index(m->rowCount() - 1, 0);
            m->setData(index, prompt);
        }
        emit promptEnteredUpdateModels(prompt);
    }

    void ChatView::onDownloadFinished() {
    }
} // namespace kaho
