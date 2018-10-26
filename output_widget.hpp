#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QString>


#include <string>

class OutputWidget : public QWidget {
  Q_OBJECT

public:
  OutputWidget(QWidget* parent = nullptr);

private:
  QGraphicsView * view;

  QLayout * layout;

public slots:
  void getError(std::string error);
  void getResult(std::string result);
  void getPlot();
};

#endif
