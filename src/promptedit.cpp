#include <kaho/promptedit.h>
#include <QGraphicsDropShadowEffect>
#include <QKeyEvent>

#include <QApplication>
#include <QPalette>
#include <QColor>
#include <QPainter>

namespace kaho {
    QColor getLighterBackgroundColor() {
        // Get the application's palette
        QPalette palette = QApplication::palette();

        // Get the background color for the Window role
        QColor bgColor = palette.color(QPalette::Window);

        // Make the color 5 units lighter
        // The lighter() function takes a percentage, so 105% is used to lighten by 5%
        QColor lighterColor = bgColor.lighter(125);

        return lighterColor;
    }

    PromptEdit::PromptEdit() {
        this->setPlaceholderText("Type a question");
    }

    void PromptEdit::keyPressEvent(QKeyEvent *event) {
        if (event->key() == Qt::Key_Return && event->modifiers() == Qt::ShiftModifier) {
            qDebug() << "PromptEdit::keyPressEvent => TODO: handle <Shift-Enter>";
        } else if (event->key() == Qt::Key_Return) {
            qDebug() << "PromptEdit::keyPressEvent => " << this->toPlainText();
            emit promptEntered(this->toPlainText());
        } else {
            QTextEdit::keyPressEvent(event);
        }
    }
}