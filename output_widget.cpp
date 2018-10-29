// output widget
#include "output_widget.hpp"
#include <iostream>

#include <QVBoxLayout>
#include <QLayout>
#include <Qt>
#include <QAbstractScrollArea>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QPen>
#include <QBrush>

OutputWidget::OutputWidget(QWidget* parent) : QWidget(parent) {
  view = new QGraphicsView();
  scene = new QGraphicsScene(view);
  layout = new QVBoxLayout();
  layout->addWidget(view);
  setLayout(layout);
}


void OutputWidget::getError(std::string error) {

  QGraphicsTextItem * errorMessage = new QGraphicsTextItem;
  errorMessage->setPlainText(QString::fromStdString(error));
  errorMessage->setPos(0,0);

  scene = new QGraphicsScene();
  scene->addItem(errorMessage);

  view->setScene(scene);
  layout->update();

}


void OutputWidget::getResult(std::string result) {

    QGraphicsTextItem * resultMessage = new QGraphicsTextItem;
    resultMessage->setPlainText(QString::fromStdString(result));
    resultMessage->setPos(0,0);

    scene = new QGraphicsScene(view);
    scene->addItem(resultMessage);

    view->setScene(scene);
    layout->update();

}


void OutputWidget::getPoint(Expression exp) {

  int x, y = 0;
  double diameter = 0.0;
  QPen pen(Qt::black);
  x = exp.getTail().at(0).head().asNumber();
  y = exp.getTail().at(1).head().asNumber();

  if(exp.property_list.find("\"size\"") != exp.property_list.end()) {
    diameter = exp.get_property("\"size\"").head().asNumber();

    if(diameter < 0.0) {
      getError("Error in point size: negative number given.");
      return;
    }

    scene->addEllipse(x, y, diameter, diameter, pen, QBrush(Qt::black));
  }
  else
    scene->addEllipse(x, y, 0, 0, pen, QBrush(Qt::black));

  view->setScene(scene);
  layout->update();
}


void OutputWidget::getLine(Expression exp) {
  int x1, x2, y1, y2 = 0;
  int width = 0;

  if(exp.property_list.find("\"thickness\"") != exp.property_list.end())
    width = exp.get_property("\"thickness\"").head().asNumber();

  QPen pen(Qt::black);
  pen.setWidth(width);

  x1 = exp.getTail().at(0).getTail().at(0).head().asNumber();
  y1 = exp.getTail().at(0).getTail().at(1).head().asNumber();
  x2 = exp.getTail().at(1).getTail().at(0).head().asNumber();
  y2 = exp.getTail().at(1).getTail().at(1).head().asNumber();

  scene->addLine(x1, y1, x2, y2, pen);
  view->setScene(scene);
  layout->update();

}


void OutputWidget::getText(Expression exp) {

  int x, y = 0;
  if(exp.property_list.find("\"position\"") != exp.property_list.end()) {
    Expression position = exp.get_property("\"position\"");
    x = position.getTail().at(0).head().asNumber();
    y = position.getTail().at(1).head().asNumber();
  }
  std::cout << x << ' ' << y << '\n';
  std::string message = exp.head().asSymbol();
  message = message.substr(1, message.size()-2);
  QGraphicsTextItem * textMessage = new QGraphicsTextItem;
  textMessage->setPlainText(QString::fromStdString(message));
  textMessage->setPos(x,y);

  scene->addItem(textMessage);
  view->setScene(scene);
  layout->update();
}
