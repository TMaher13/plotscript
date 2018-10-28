// output widget
#include "output_widget.hpp"
#include <iostream>

#include <QVBoxLayout>
#include <QLayout>
#include <Qt>
#include <QAbstractScrollArea>

OutputWidget::OutputWidget(QWidget* parent) : QWidget(parent) {

  view = new QGraphicsView();
  scene = new QGraphicsScene(view);
  layout = new QVBoxLayout();
  layout->addWidget(view);
  setLayout(layout);

}

void OutputWidget::getError(std::string error) {
  //width = view->frameSize().width();
  //height = view->frameSize().height();

  //layout->removeWidget(view);
  QGraphicsTextItem * errorMessage = new QGraphicsTextItem;
  errorMessage->setPlainText(QString::fromStdString(error));
  errorMessage->setPos(0,0);

  scene->clear();
  scene = new QGraphicsScene();
  scene->addItem(errorMessage);

  view->setScene(scene);
  layout->update();

}


void OutputWidget::getResult(std::string result) {

  //layout->removeWidget(view);
  QGraphicsTextItem * resultMessage = new QGraphicsTextItem;
  resultMessage->setPlainText(QString::fromStdString(result));
  resultMessage->setPos(0,0);

  scene->clear();
  scene = new QGraphicsScene(view);
  scene->addItem(resultMessage);

  view->setScene(scene);
  layout->update();
}


void OutputWidget::getPoint(Expression exp) {

  std::cout << "Point\n";
}

void OutputWidget::getLine(Expression exp) {


}

void OutputWidget::getText(Expression exp) {

  std::string message = exp.head().asSymbol();
  message = message.substr(1, message.size()-2);
  QGraphicsTextItem * textMessage = new QGraphicsTextItem;
  textMessage->setPlainText(QString::fromStdString(message));
  textMessage->setPos(0,0);

  scene->clear();
  scene = new QGraphicsScene(view);
  scene->addItem(textMessage);

  view->setScene(scene);
  layout->update();
}
