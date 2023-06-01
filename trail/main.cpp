#include "mainwindow.h"
#include "index.h"

#include <QApplication>
#include <QApplication>
#include <QGraphicsScene>



int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    //return a.exec();

    QApplication app(argc, argv);

    QGraphicsScene scene;
    GameView view(&scene);

    view.show();

    return app.exec();
}
