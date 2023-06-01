#ifndef INDEX_H
#define INDEX_H

#include <QGraphicsView>

class GameView : public QGraphicsView
{
public:
    GameView(QGraphicsScene *scene);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    enum GameState { INTRO, PLAYING, SHOPPING, WIN } gameState = INTRO;
};

#endif // INDEX_H
