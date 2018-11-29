// input widget implementation

#include <QPlainTextEdit>
#include <QWidget>
#include <QKeyEvent>
#include <QTextCursor>
#include <QDebug>
#include <Qt>
#include <QString>

#include "input_widget.hpp"


InputWidget::InputWidget(QWidget* parent) : QPlainTextEdit(parent) {

}

void InputWidget::setPlainText(QString text) {
  this->clear();
  this->appendPlainText(text);
}


void InputWidget::keyPressEvent(QKeyEvent * event) {

  if (event->type()==QEvent::KeyPress) {
    QKeyEvent* key = static_cast<QKeyEvent*>(event);
    if(key->key()==Qt::Key_Return) {
      if (event->modifiers() & Qt::ShiftModifier) {
        //qDebug() << "Sending input to NotebookApp";
        notebookCmd = toPlainText().toStdString();
        emit sendInput(notebookCmd);
      }
      else
        this->insertPlainText(QString::fromStdString("\n"));
    }
    else if(key->key() == Qt::Key_Backspace)
      this->textCursor().deletePreviousChar();
    else
      this->insertPlainText(event->text());
  }
}
