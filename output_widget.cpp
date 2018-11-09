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
#include <QFont>

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

  int x = 0;
  int y = 0;
  int diameter = 0;
  QPen pen(Qt::black);

  if(!exp.getTail().at(0).head().isNumber() || !exp.getTail().at(1).head().isNumber()) {
    getError("Error in point location: point is not a Number.");
    return;
  }

  x = exp.getTail().at(0).head().asNumber();
  y = exp.getTail().at(1).head().asNumber();

  if(exp.property_list.find("\"size\"") != exp.property_list.end()) {

    if(!exp.get_property("\"size\"").head().isNumber()) {
      getError("Error in point size: size is not a Number.");
      return;
    }

    if(diameter < 0) {
      getError("Error in point size: negative Number given.");
      return;
    }
    diameter = exp.get_property("\"size\"").head().asNumber();

    scene->addEllipse(x - diameter/2, y - diameter/2, diameter, diameter, pen, QBrush(Qt::black));
  }
  else
    scene->addEllipse(x, y, 0, 0, pen, QBrush(Qt::black));

  //scene->setSceneRect(200,200,200,200);
  view->setScene(scene);
  layout->update();
}


void OutputWidget::getLine(Expression exp) {
  int x1 = 0;
  int x2 = 0;
  int y1 = 0;
  int y2 = 0;
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

  int x = 0;
  int getCenter = exp.head().asSymbol().length() / 2;
  int y = - getCenter;

  if(exp.property_list.find("\"position\"") != exp.property_list.end()) {
    if(exp.get_property("\"position\"").getTail().size() != 2) {
      emit getError("Error in call to make-text: invalid argument");
      return;
    }

    if(exp.get_property("\"position\"").get_property("\"object-name\"").head().asString() != "\"point\"") {
      emit getError("Error in call to make-text: postion is not a make-point object");
      return;
    }

    Expression position = exp.get_property("\"position\"");
    x += position.getTail().at(0).head().asNumber();
    y = position.getTail().at(1).head().asNumber();
  }
  std::string message = exp.head().asSymbol();
  message = message.substr(1, message.size()-2);
  textMessage = new QGraphicsTextItem();
  textMessage->setPlainText(QString::fromStdString(message));
  textMessage->setPos(x,y);

  QFont newFont("Courier", 1);
  //set font of application
  textMessage->setFont(newFont);

  scene->setSceneRect(textMessage->sceneBoundingRect());
  scene->addItem(textMessage);
  view->setScene(scene);
  layout->update();
}
