#include "mainwindow.h"
#include "index.h"
#include "action.h"
#include "behavior.h"
#include "game.h"

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

    //enegine文件函数调用示例
    // 绘制游戏画面
       clear();
       drawSprite({0, 0, 32, 32}, 100, 100);
       write("Hello, world!", 10, 10);
    //game文件函数调用示例
    GameObject player;
    Game game(&player);

    game.update(1000);
    //action文件函数调用示例


    damage(&game, &player, 10);
    die(&game, &player);
    cast(&game);
    resurrect(&game);

    //behavior文件函数调用示例
    player.addBehaviour(new Attack(&player));
    player.addBehaviour(new DespawnTimer(&player, 1000));
    player.addBehaviour(new March(&player, 10));
    player.addBehaviour(new Damaging(&player));


    return app.exec();
}
