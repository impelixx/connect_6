#ifndef GAMEBOARDWIDGETIOS_H
#define GAMEBOARDWIDGETIOS_H

#include "gui/GameBoardWidget.h"

class GameBoardWidgetIOS : public GameBoardWidget {
    Q_OBJECT

public:
    explicit GameBoardWidgetIOS(QWidget* parent = nullptr);
    ~GameBoardWidgetIOS();

    void setupUI();
    void updateBoard();
    void updateStatus();
    void makeAIMove();
    void handleCellClicked(int row, int col);
    void getHint();
    void highlightWinningLine();

private:
    void* iosView; // Opaque pointer to GameBoardView
};

#endif // GAMEBOARDWIDGETIOS_H 