// output widget
#include "output_widget.hpp"

#include <QVBoxLayout>
#include <QLayout>

OutputWidget::OutputWidget(QWidget* parent) : QWidget(parent) {

  view = new QGraphicsView();
  QLayout * layout = new QVBoxLayout();
  layout->addWidget(view);
  setLayout(layout);

}
