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


void OutputWidget::getResult(std::string result) { //}, bool isDefined) {

  //if(!isDefined) {
    QGraphicsTextItem * resultMessage = new QGraphicsTextItem;
    resultMessage->setPlainText(QString::fromStdString(result));
    resultMessage->setPos(0,0);

    scene = new QGraphicsScene(view);
    scene->addItem(resultMessage);

    view->setScene(scene);
    layout->update();
  //}

}


void OutputWidget::getPoint(Expression exp) {

  /*QGraphicsEllipseItem* circle = new QGraphicsEllipseItem();
  circle->setRect(-5,-5,5,5);
  circle->setStartAngle(5760);
  circle->setSpanAngle(5760);
  scene->addItem(circle);*/
  int x, y = 0;
  double diameter = 0.0;
  QPen pen(Qt::black);
  x = exp.getTail().at(0).head().asNumber();
  y = exp.getTail().at(1).head().asNumber();

  if(exp.property_list.find("\"size\"") != exp.property_list.end()) {
    diameter = exp.get_property("\"size\"").head().asNumber();
    //std::cout << exp.property_list["\"size\""];
    std::cout << diameter << " I am here!\n";
    scene->addEllipse(x, y, diameter, diameter, pen, QBrush(Qt::black));
  }
  else {
    std::cout << "Can\'t find it\n";
    scene->addEllipse(x, y, 0, 0, pen, QBrush(Qt::black));
  }

  view->setScene(scene);
  layout->update();
}

void OutputWidget::getLine(Expression exp) {


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

  //scene->clear();
  scene = new QGraphicsScene(view);
  scene->addItem(textMessage);

  view->setScene(scene);
  layout->update();
}
