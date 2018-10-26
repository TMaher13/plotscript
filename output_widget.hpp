#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>


#include <string>

class OutputWidget : public QWidget {
  Q_OBJECT

public:
  OutputWidget(QWidget* parent = nullptr);

private:
  QGraphicsView * view;

protected slots:
  //void getError();
  //void getResult();
  //void getPlot();
};

#endif
