#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <vector>
#include <string>
#include <memory>
#include <QObject>
#include "core/Player.h"
#include "core/GameState.h"

class GameBoard : public QObject {
    Q_OBJECT

public:
    static const int BOARD_SIZE = 15;
    static const int WIN_LENGTH = 6;

    GameBoard();
    ~GameBoard() = default;

    GameBoard& operator=(const GameBoard& other);

    bool makeMove(int row, int col, Player player);
    bool isValidMove(int row, int col) const;
    GameState getGameState() const;
    Player getCurrentPlayer() const;
    Player getCell(int row, int col) const;
    void reset();
    void undoLastMove();
    std::vector<std::pair<int, int>> getValidMoves() const;
    std::string getMoveHistory() const;
    void saveGame(const std::string& filename) const;
    bool loadGame(const std::string& filename);
    std::pair<int, int> getHint() const;
    void undoMove();
    std::vector<std::pair<int, int>> getWinningLine() const;

signals:
    void boardChanged();
    void gameStateChanged(GameState state);
    void moveMade(int row, int col, Player player);

private:
    std::vector<std::vector<Player>> board;
    std::vector<std::tuple<int, int, Player>> moveHistory;
    Player currentPlayer;
    GameState gameState;
    std::vector<std::pair<int, int>> winningLine;

    bool checkWin(int row, int col, Player player);
    bool checkDirection(int row, int col, int dRow, int dCol, Player player) const;
    bool checkLine(int row, int col, int dRow, int dCol, Player player);
    void updateWinningLine(int row, int col, int dRow, int dCol, Player player);
};

#endif // GAMEBOARD_H 