#ifndef INPUT_WIDGET_HPP
#define INPUT_WIDGET_HPP

#include <string>

#include <QPlainTextEdit>
#include <QWidget>



class InputWidget: public QPlainTextEdit {
Q_OBJECT

public:
  InputWidget(QWidget* parent = nullptr);
  //virtual ~InputWidget() {};

  void keyPressEvent(QKeyEvent * event);

  void setPlainText(QString text);

private:
  std::string notebookCmd;

signals:
  void sendInput(std::string NotebookCmd);


};

#endif
