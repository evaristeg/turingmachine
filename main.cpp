#include "MainWidget.hpp"
#include <QApplication>
#include <QMainWindow>

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    QMainWindow window;
    MainWidget * widget = new MainWidget(&window);
    window.setCentralWidget(widget);
    window.show();
    return app.exec();
}

