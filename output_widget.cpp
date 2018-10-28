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

  layout->removeWidget(view);
}

void OutputWidget::getLine(Expression exp) {

  layout->removeWidget(view);
}

void OutputWidget::getText(Expression exp) {

  layout->removeWidget(view);
}
