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



NotebookApp::NotebookApp(QWidget* parent) : QWidget(parent) {
  std::ifstream ifs(STARTUP_FILE);
  if(interp.parseStream(ifs)){
    Expression startup_exp = interp.evaluate();
  }

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

  auto layout = new QVBoxLayout();
  layout->addWidget(input);
  layout->addWidget(output);
  layout->setSizeConstraint(QLayout::SetFixedSize);

  setLayout(layout);
}

void NotebookApp::input_cmd(std::string NotebookCmd) {
  std::stringstream stream(NotebookCmd);

  if(!interp.parseStream(stream)){
    emit sendError("Invalid Expression. Could not parse.");
    //std::cout << "Invalid Expression. Could not parse.\n";
  }
  else{
    try{
      Expression exp = interp.evaluate();
      // Send exp to output_widget
      std::string name = "\"object-name\"";
      if(exp.isHeadList()) {
        if(exp.property_list.find(name) == exp.property_list.end()) {
          std::ostringstream result;
          result << exp;
          std::string resultStr = result.str();
          resultStr = resultStr.substr(1, resultStr.size()-2);
          emit sendResult(resultStr);
        }
        else if(exp.get_property(name) == Expression(Atom("\"point\""))) {
          emit sendPoint(exp);
        }
        else if(exp.get_property(name) == Expression(Atom("\"line\""))) {
          sendLine(exp);
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
        emit sendResult(resultStr);
      }
    }
    catch(const SemanticError & ex){
        // Send ex.what() to output_widget
        //qDebug() << ex.what();
        emit sendError(ex.what());
        //std::cerr << ex.what() << std::endl;
    }
  }
}
