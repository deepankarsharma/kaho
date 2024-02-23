#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QStringListModel>
#include <QLineEdit>
#include <QPushButton>
#include <QListView>
#include <QTextEdit>
#include <kaho/promptedit.h>
#include <QProgressBar>

namespace kaho {
    class ChatView : public QWidget {
    Q_OBJECT

    public:
        ChatView(QWidget *parent = nullptr);

    private:
        void initializeUi();

    signals:
        void questionReceived(const QString& question);
        void promptEnteredUpdateModels(const QString& question);



    public slots:
        void promptEnteredUpdateUi(const QString &prompt);
        void onDownloadFinished();
    private:
        QProgressBar* m_progress_bar;
        QListView* m_view_questions;
        QTextEdit* m_view_current_answer;
        PromptEdit* m_view_prompt;
        QString m_answer;
    };

}
