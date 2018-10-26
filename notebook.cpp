#include <QApplication>
#include <QWidget>
#include <QLayout>
#include <QPlainTextEdit>
#include <QDebug>

#include "notebook_app.hpp"
#include "input_widget.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  NotebookApp Notebook; // = new NotebookApp();
  //InputWidget input;

  Notebook.show();
  //input.show();

  return app.exec();
}
