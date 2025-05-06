#ifndef GAMEBOARDWIDGET_H
#define GAMEBOARDWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QComboBox>
#include "core/GameBoard.h"
#include "ai/GameAI.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QLabel;
class QComboBox;

class GameBoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit GameBoardWidget(GameBoard* board, QWidget* parent = nullptr);
    ~GameBoardWidget();

    void updateBoard();
    void setGameMode(int mode);
    void setHumanPlayer(Player player);
    void setAIDifficulty(AIDifficulty difficulty);

signals:
    void returnToMainMenu();
    void gameStateChanged(GameState state);

private slots:
    void handleCellClicked(int row, int col);
    void undoMove();
    void getHint();
    void saveGame();
    void loadGame();
    void resetGame();
    void makeAIMove();
    void onMenuClicked();
    void onDifficultyChanged(int index);
    void onAIDifficultyChanged(int index);
    void onPauseClicked();
    void highlightWinningLine();

private:
    GameBoard* board;
    GameAI* ai;
    GameAI* ai2;  // Second AI for AI vs AI mode
    QGridLayout* gridLayout;
    QVBoxLayout* mainLayout;
    QHBoxLayout* controlLayout;
    QPushButton* undoButton;
    QPushButton* hintButton;
    QPushButton* saveButton;
    QPushButton* loadButton;
    QPushButton* resetButton;
    QPushButton* menuButton;
    QPushButton* pauseButton;
    QLabel* gameModeLabel;
    QLabel* difficultyLabel;
    QLabel* currentPlayerLabel;
    QComboBox* difficultyComboBox;
    QComboBox* ai2DifficultyComboBox; 
    QPushButton* hintCell;
    int gameMode;
    Player humanPlayer;

    void setupUI();
    void setupConnections();
    void updateStatus();
};

#endif // GAMEBOARDWIDGET_H 