// notebook_app.hpp
#ifndef NOTEBOOK_APP_HPP
#define NOTEBOOK_APP_HPP

#include <QString>
#include <QPushButton>

#include <sstream>
#include <thread>
#include <csignal>

#include "interpreter.hpp"
#include "semantic_error.hpp"

#include "input_widget.hpp"
#include "output_widget.hpp"

#include "thread_safe_queue.hpp"
#include "interpreter_thread.hpp"
#include "output_thread.hpp"


class NotebookApp : public QWidget {
  Q_OBJECT

public:
  NotebookApp(QWidget* parent = nullptr);

  virtual ~NotebookApp() {
    if(interpRunning) {
      input_queue.push("%stop");
      int_th.join();
    }
  }

private:
  QString notebookCmd;

  Interpreter interp;

  InputWidget* input;
  OutputWidget* output;

  // PushButtons for kernel commands
  QPushButton* startButton;
  QPushButton* stopButton;
  QPushButton* resetButton;
  QPushButton* interruptButton;

  // Input/Output queues and interpreter/output threads
  ThreadSafeQueue<std::string> input_queue;
  ThreadSafeQueue<output_type> output_queue;
  InterpreterThread interpThread; //(&input_queue, &output_queue, interp);
  std::thread int_th;


  bool isDefined;
  bool interpRunning;

protected slots:
  void input_cmd(std::string NotebookCmd);

  void handle_start();
  void handle_stop();
  void handle_reset();
  void handle_interrupt();


signals:
  void sendError(std::string error);
  void sendResult(std::string result); //, bool isDefined);
  void sendPoint(Expression exp);
  void sendLine(Expression exp);
  void sendText(Expression exp);



};

#endif
