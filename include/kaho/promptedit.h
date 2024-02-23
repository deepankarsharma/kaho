#pragma once
#include <QTextEdit>

namespace kaho {
    class PromptEdit : public QTextEdit
    {
    Q_OBJECT
    public:
        PromptEdit();
    protected:
        void keyPressEvent(QKeyEvent *event) override;
    signals:
        void promptEntered(const QString &text);

    };

} // namespace kaho

