// output widget
#include "output_widget.hpp"
#include <iostream>

#include <QVBoxLayout>
#include <QLayout>

OutputWidget::OutputWidget(QWidget* parent) : QWidget(parent) {

  view = new QGraphicsView();
  layout = new QVBoxLayout();
  layout->addWidget(view);
  setLayout(layout);

}

void OutputWidget::getError(std::string error) {
  layout->removeWidget(view);
  QGraphicsTextItem * errorMessage = new QGraphicsTextItem;
  errorMessage->setPlainText(QString::fromStdString(error));
  errorMessage->setPos(0,0);

  QGraphicsScene *errorScene = new QGraphicsScene;
  errorScene->addItem(errorMessage);

  view = new QGraphicsView(errorScene);

  layout->addWidget(view);
  setLayout(layout);
  layout->update();

}


void OutputWidget::getResult(std::string result) {

  layout->removeWidget(view);
  QGraphicsTextItem * resultMessage = new QGraphicsTextItem;
  resultMessage->setPlainText(QString::fromStdString(result));
  resultMessage->setPos(0,0);

  QGraphicsScene *resultScene = new QGraphicsScene;
  resultScene->addItem(resultMessage);

  view = new QGraphicsView(resultScene);

  layout->addWidget(view);
  setLayout(layout);
  layout->update();
}


void OutputWidget::getPlot() {

  layout->removeWidget(view);
}
