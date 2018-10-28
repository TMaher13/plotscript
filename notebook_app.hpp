// notebook_app.hpp
#ifndef NOTEBOOK_APP_HPP
#define NOTEBOOK_APP_HPP

#include <QString>

#include <sstream>
#include "interpreter.hpp"
#include "semantic_error.hpp"

#include "input_widget.hpp"
#include "output_widget.hpp"


class NotebookApp : public QWidget {
  Q_OBJECT

public:
  NotebookApp(QWidget* parent = nullptr);

private:
  QString notebookCmd;

  Interpreter interp;

  InputWidget* input;
  OutputWidget* output;

protected slots:
  void input_cmd(std::string NotebookCmd);


signals:
  void sendError(std::string error);
  void sendResult(std::string result);
  void sendPoint(Expression exp);
  void sendLine(Expression exp);
  void sendText(Expression exp);



};

#endif
