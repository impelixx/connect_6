#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "core/GameBoard.h"
#include "gui/GameBoardWidget.h"

class QStackedWidget;
class MainMenuWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void startGame(int gameMode, int difficulty, bool playAsBlack);
    void showMainMenu();

private:
    QStackedWidget* stackedWidget;
    MainMenuWidget* mainMenu;
    GameBoard* gameBoard;
    GameBoardWidget* gameBoardWidget;
};

#endif // MAINWINDOW_H 