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


class ChatView : public QWidget {
    Q_OBJECT

public:
    ChatView(QWidget *parent = nullptr);

private:
    void initializeUi();

signals:
    void questionReceived(const QString& question);

public slots:
    void updateQuestionsView(const QString &prompt);
private:
    QListView* m_view_questions;
    QTextEdit* m_view_current_question;
    QTextEdit* m_view_current_answer;
    PromptEdit* m_view_prompt;
};
