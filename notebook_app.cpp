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



NotebookApp::NotebookApp(QWidget* parent) : QWidget(parent) {
  std::ifstream ifs(STARTUP_FILE);
  if(interp.parseStream(ifs))
    Expression startup_exp = interp.evaluate();

  input = new InputWidget();
  QObject::connect(input, &InputWidget::sendInput, this, &NotebookApp::input_cmd);

  output = new OutputWidget();
  //QObject::connect(this,&NotebookApp::sendError, output, &OutputWidget::getError);
  //QObject::connect(this,&NotebookApp::sendResult, output, &OutputWidget::getResult);
  //QObject::connect(this,&NotebookApp::sendPlot, output, &OutputWidget::getPlot);

  //QSplitter* splitter = new QSplitter(Qt::Vertical, parent);
  //splitter->addWidget(input);
  //splitter->addWidget(output);

  auto layout = new QVBoxLayout();
  layout->addWidget(input);
  layout->addWidget(output);

  setLayout(layout);
  //loadStartup(layout);
}

void NotebookApp::input_cmd(std::string NotebookCmd) {
  //std::string inputToString = NotebookCmd.toStdString();
  std::stringstream stream(NotebookCmd);
  //std::cout << "Text received: " << NotebookCmd << '\n';

  if(!interp.parseStream(stream)){
    //error("Invalid Expression. Could not parse.");
    std::cout << "Invalid Expression. Could not parse.\n";
  }
  else{
    try{
      Expression exp = interp.evaluate();
      // Send exp to output_widget
      std::cout << exp << '\n';
      //std::cout << exp << std::endl;
    }
    catch(const SemanticError & ex){
        // Send ex.what() to output_widget
        qDebug() << ex.what();
        //std::cerr << ex.what() << std::endl;
    }
  }

}


// Overwrite KeyPressEvent so that it can read Shift+Enter
/*void Notebook_App::KeyPressEvent(QKeyEvent * event) {
  switch (event->key()) {
    case (Qt::Key_Enter):
      if (event->modifiers() & Qt::ShiftModifier)
        qDebug()<<"shift and one";
      break;
  }
}*/
