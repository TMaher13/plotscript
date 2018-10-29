#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QString>

#include "expression.hpp"


#include <string>

class OutputWidget : public QWidget {
  Q_OBJECT

public:
  OutputWidget(QWidget* parent = nullptr);

  QGraphicsScene *scene;

private:
  QGraphicsView * view;
  QLayout * layout;

  QGraphicsTextItem * textMessage;

public slots:
  void getError(std::string error);
  void getResult(std::string result);
  void getPoint(Expression exp);
  void getLine(Expression exp);
  void getText(Expression exp);
};

#endif
