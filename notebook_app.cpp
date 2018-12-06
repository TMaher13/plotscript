// notebook_app.cpp
#include <QLayout>
#include <QWidget>
#include <QSplitter>
#include <QString>
#include <QDebug>
#include <QVBoxLayout>
#include <QThread>
#include <QTimer>

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include "notebook_app.hpp"
#include "startup_config.hpp"
#include "expression.hpp"

extern bool isInterrupted;



NotebookApp::NotebookApp(QWidget* parent) : QWidget(parent), isDefined(false), isError(false), caughtInterrupt(false) {

  isInterrupted = false;
  // PushButtons for GUI kernel commands
  startButton = new QPushButton("Start Kernel");
  startButton->setObjectName("start");
  stopButton = new QPushButton("Stop Kernel");
  stopButton->setObjectName("stop");
  resetButton = new QPushButton("Reset Kernel");
  resetButton->setObjectName("reset");
  interruptButton = new QPushButton("Interrupt");
  interruptButton->setObjectName("interrupt");

  event_timer = new QTimer(this); //::singleShot(100, this, SLOT(interrupt_timer_loop()));
  connect(event_timer, SIGNAL(timeout()), this, SLOT(interrupt_timer_loop()));
  //event_timer->stop();
  //connect(event_timer, )
  // Create connections for button signals/slots
  connect(startButton, SIGNAL (released()), this, SLOT (handle_start()));
  connect(stopButton, SIGNAL (released()), this, SLOT (handle_stop()));
  connect(resetButton, SIGNAL (released()), this, SLOT (handle_reset()));
  connect(interruptButton, SIGNAL (released()), this, SLOT (handle_interrupt()));

  input = new InputWidget();
  input->setObjectName("input");
  QObject::connect(input, &InputWidget::sendInput, this, &NotebookApp::input_cmd);

  output = new OutputWidget();
  output->setObjectName("output");

  // Create connections b/w signals and slots
  QObject::connect(this,&NotebookApp::sendError, output, &OutputWidget::getError);
  QObject::connect(this,&NotebookApp::sendResult, output, &OutputWidget::getResult);
  QObject::connect(this,&NotebookApp::sendPoint, output, &OutputWidget::getPoint);
  QObject::connect(this,&NotebookApp::sendLine, output, &OutputWidget::getLine);
  QObject::connect(this,&NotebookApp::sendText, output, &OutputWidget::getText);

  // Add buttons to layout
  auto layoutButtons = new QHBoxLayout();
  layoutButtons->addWidget(startButton);
  layoutButtons->addWidget(stopButton);
  layoutButtons->addWidget(resetButton);
  layoutButtons->addWidget(interruptButton);

  // Add button layout and widgets to layout
  auto layout = new QVBoxLayout();
  layout->addLayout(layoutButtons);
  layout->addWidget(input);
  layout->addWidget(output);
  layout->setSizeConstraint(QLayout::SetFixedSize);

  setLayout(layout);

  std::ifstream ifs(STARTUP_FILE);
  if(interp.parseStream(ifs))
    Expression startup_exp = interp.evaluate();
  else
    emit sendError("Error: Invalid Expression. Could not parse.");

  interpThread = InterpreterThread(&input_queue, &output_queue, interp);
  int_th = std::thread(interpThread);
  interpRunning = true;
}

void NotebookApp::input_cmd(std::string NotebookCmd) {
  std::stringstream stream(NotebookCmd);
  output->scene->clear();

  //std::cout << interpRunning << '\n';
  if(!interpRunning) {
    //std::cout << "Here\n";
    emit sendError("Error: interpreter kernel not running");
    return;
  }

  if(!interp.parseStream(stream)) {
    emit sendError("Error: Invalid Expression. Could not parse in GUI.");
  }
  else {
    input_queue.push(NotebookCmd);
    //input->setEnabled(false);

    // Call the timer to try and pop from the output_queue
    event_timer->start(50);
    //handle_interrupt();
  }
}

void NotebookApp::interrupt_timer_loop() {
    if(output_queue.try_pop(result)) {
      event_timer->stop();
      //input->setEnabled(true);
      //std::cout << "Popped\n";
      if(result.isError) {
        emit sendError(result.err_result.what());
        return;
      }
      else
        exp = Expression(result.exp_result);
      //std::cout << "After popped\n";
      try {
        if(caughtInterrupt) {
          input_queue.push("%reset");
          int_th.join();

          interp = Interpreter();

          interpThread = InterpreterThread(&input_queue, &output_queue, interp);
          int_th = std::thread(interpThread);
          caughtInterrupt = false;
        }

        if(isError) {
          isError = false;
          return;
        }

        //std::cout << "Result: " << exp << '\n';
        //std::cout << "Result type is none: " << exp.head().isNone() << '\n';

        // Evaluate result expression to know how to send result to output widget
        std::string name = "\"object-name\"";
        if(exp.isHeadList()) {

          if(exp.property_list.find(name) != exp.property_list.end()) {
            if(exp.get_property(name) == Expression(Atom("\"point\""))) {
              emit sendPoint(exp);
            }
            if(exp.get_property(name) == Expression(Atom("\"line\""))) {
              emit sendLine(exp);
            }
          }
          else {

            bool isPlot = false;

            for(auto & item : exp.getTail()) {
              if(item.get_property(name) == Expression(Atom("\"point\""))) {
                emit sendPoint(item);
                isPlot = true;
              }
              else if(item.get_property(name) == Expression(Atom("\"line\""))) {
                emit sendLine(item);
                isPlot = true;
              }
              else if(item.get_property(name) == Expression(Atom("\"text\""))) {
                emit sendText(item);
                isPlot = true;
              }
            }

            if(!isPlot) {
              std::ostringstream result;
              result << exp;

              std::string resultStr = result.str();
              resultStr = resultStr.substr(1, resultStr.size()-2);
              emit sendResult(resultStr);
            }
          }
        }
        else if((exp.property_list.find(name)!=exp.property_list.end()) && (exp.get_property(name) == Expression(Atom("\"text\"")))) {
          emit sendText(exp);
        }
        else {
          // Otherwise send result to output widget
          std::ostringstream streamResult;
          streamResult << exp;
          std::string resultStr = streamResult.str();
          if(!exp.isHeadLambda())
            emit sendResult(resultStr);
        }
      }
      catch(const SemanticError & ex) {
          emit sendError(ex.what());
      }


    }
}

void NotebookApp::handle_start() {
  if(!interpRunning) {
    interp = Interpreter();

    interpThread = InterpreterThread(&input_queue, &output_queue, interp);
    int_th = std::thread(interpThread);
  }
  interpRunning = true;
}

void NotebookApp::handle_stop() {
  if(interpRunning) {
    input_queue.push("%stop");
    int_th.join();
  }
  interpRunning = false;
}

void NotebookApp::handle_reset() {
  if(interpRunning) {
    input_queue.push("%reset");
    int_th.join();
  }
  interp = Interpreter();

  std::ifstream ifs(STARTUP_FILE);
  if(interp.parseStream(ifs))
    Expression startup_exp = interp.evaluate();

  interpThread = InterpreterThread(&input_queue, &output_queue, interp);
  int_th = std::thread(interpThread);

  interpRunning = true;
}

void NotebookApp::handle_interrupt() {
  if(interpRunning) {
    isInterrupted = true;
    //std::cout << "Caught interrupt in slot.\n";
    caughtInterrupt = true;
  }
  else
    isInterrupted = false;
}
