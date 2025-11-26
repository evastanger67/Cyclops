#include <QApplication>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>
#include "main_visor.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  app.setStyle(QStyleFactory::create("Fusion"));
  QPalette lightPalette;
  lightPalette.setColor(QPalette::Window, Qt::gray);
  lightPalette.setColor(QPalette::WindowText, Qt::white);
  lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));
  lightPalette.setColor(QPalette::AlternateBase, QColor(233, 231, 227));
  lightPalette.setColor(QPalette::ToolTipBase, Qt::white);
  lightPalette.setColor(QPalette::ToolTipText, Qt::black);
  lightPalette.setColor(QPalette::Text, Qt::black);
  lightPalette.setColor(QPalette::Button, QColor(240, 240, 240));
  lightPalette.setColor(QPalette::ButtonText, Qt::black);
  lightPalette.setColor(QPalette::BrightText, Qt::red);
  lightPalette.setColor(QPalette::Link, QColor(0, 0, 255));
  lightPalette.setColor(QPalette::Highlight, QColor(0, 120, 215));
  lightPalette.setColor(QPalette::HighlightedText, Qt::white);
  app.setPalette(lightPalette);
  MainVisor mainWindow;
  mainWindow.show();

  return app.exec();
}
