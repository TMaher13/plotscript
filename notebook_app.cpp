// notebook_app.cpp
#include <QLayout>
#include <QWidget>
#include <QSplitter>
#include <QString>
#include <QDebug>
#include <QVBoxLayout>

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include "notebook_app.hpp"
#include "startup_config.hpp"
#include "expression.hpp"



NotebookApp::NotebookApp(QWidget* parent) : QWidget(parent), isDefined(false) {

  interpThread = InterpreterThread(&input_queue, &output_queue, interp);
  int_th = std::thread(interpThread);
  interpRunning = true;

  // PushButtons for GUI kernel commands
  startButton = new QPushButton("Start Kernel");
  startButton->setObjectName("start");
  stopButton = new QPushButton("Stop Kernel");
  stopButton->setObjectName("stop");
  resetButton = new QPushButton("Reset Kernel");
  resetButton->setObjectName("reset");
  interruptButton = new QPushButton("Interrupt");
  interruptButton->setObjectName("interrupt");

  connect(startButton, SIGNAL (released()), this, SLOT (handle_start()));
  connect(stopButton, SIGNAL (released()), this, SLOT (handle_stop()));
  connect(resetButton, SIGNAL (released()), this, SLOT (handle_reset()));
  connect(interruptButton, SIGNAL (released()), this, SLOT (handle_interrupt()));

  input = new InputWidget();
  input->setObjectName("input");
  QObject::connect(input, &InputWidget::sendInput, this, &NotebookApp::input_cmd);

  output = new OutputWidget();
  output->setObjectName("output");
  QObject::connect(this,&NotebookApp::sendError, output, &OutputWidget::getError);
  QObject::connect(this,&NotebookApp::sendResult, output, &OutputWidget::getResult);
  QObject::connect(this,&NotebookApp::sendPoint, output, &OutputWidget::getPoint);
  QObject::connect(this,&NotebookApp::sendLine, output, &OutputWidget::getLine);
  QObject::connect(this,&NotebookApp::sendText, output, &OutputWidget::getText);

  //std::cout << "Built objects\n";

  auto layoutButtons = new QHBoxLayout();
  layoutButtons->addWidget(startButton);
  layoutButtons->addWidget(stopButton);
  layoutButtons->addWidget(resetButton);
  layoutButtons->addWidget(interruptButton);

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
}

void NotebookApp::input_cmd(std::string NotebookCmd) {
  std::stringstream stream(NotebookCmd);
  output->scene->clear();

  std::cout << interpRunning << '\n';
  if(!interpRunning) {
    std::cout << "Here\n";
    emit sendError("Error: interpreter kernel not running");
    return;
  }

  if(!interp.parseStream(stream)) {

    emit sendError("Error: Invalid Expression. Could not parse in GUI.");
  }
  else {
    try {
      Expression exp = interp.evaluate();
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
            //for(auto & item : exp.getTail()) {
            result << exp;
            //}

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
        std::ostringstream result;
        result << exp;
        std::string resultStr = result.str();
        //std::cout << resultStr << '\n';
        if(!exp.isHeadLambda()) // && !(exp.head().isNone() && exp.getTail().size() > 0))
          emit sendResult(resultStr); //, exp.isDefined());
      }
    }
    catch(const SemanticError & ex) {
        emit sendError(ex.what());
    }
  }
}

void NotebookApp::handle_start() {
  if(!interpRunning)
    int_th = std::thread(interpThread);
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
  int_th = std::thread(interpThread);
  interpRunning = true;

}

void NotebookApp::handle_interrupt() {

}
