// output widget
#include "output_widget.hpp"
#include <iostream>

#include <QVBoxLayout>
#include <QLayout>
#include <Qt>

OutputWidget::OutputWidget(QWidget* parent) : QWidget(parent) {

  view = new QGraphicsView();
  scene = new QGraphicsScene();
  //view->setFixedSize(view->frameSize().width(), view->frameSize().height());
  //view->setSceneRect(0, 0, width, height);
  layout = new QVBoxLayout();
  layout->addWidget(view);
  setLayout(layout);

}

void OutputWidget::getError(std::string error) {
  layout->removeWidget(view);
  QGraphicsTextItem * errorMessage = new QGraphicsTextItem;
  errorMessage->setPlainText(QString::fromStdString(error));
  errorMessage->setPos(0,0);

  scene->clear();
  scene = new QGraphicsScene;
  scene->addItem(errorMessage);
  //scene->setSceneRect(0,0,view->width(),view->height());
  view = new QGraphicsView(scene);

  layout->addWidget(view);
  setLayout(layout);
  layout->update();

}


void OutputWidget::getResult(std::string result) {

  layout->removeWidget(view);
  QGraphicsTextItem * resultMessage = new QGraphicsTextItem;
  resultMessage->setPlainText(QString::fromStdString(result));
  resultMessage->setPos(0,0);
  scene = new QGraphicsScene;
  //scene->setSceneRect(0,0,view->width(),view->height());
  scene->addItem(resultMessage);

  view = new QGraphicsView(scene);

  layout->addWidget(view);
  setLayout(layout);
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
