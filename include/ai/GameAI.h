#ifndef GAMEAI_H
#define GAMEAI_H

#include <QObject>
#include <vector>
#include <utility>
#include <random>
#include "core/GameBoard.h"
#include "core/Player.h"
#include "core/GameState.h"

enum class AIDifficulty {
    EASY,
    MEDIUM,
    HARD     
};

class GameAI : public QObject {
    Q_OBJECT

public:
    explicit GameAI(QObject* parent = nullptr);
    std::pair<int, int> getBestMove(const GameBoard& board, Player player);
    void setDifficulty(AIDifficulty difficulty);
    AIDifficulty getDifficulty() const;

private:
    std::pair<int, int> getBestMoveWithDepth(const GameBoard& board, Player player, int depth);
    int minimax(const GameBoard& board, int depth, int alpha, int beta, bool maximizing, Player player);
    int evaluateBoard(const GameBoard& board, Player player);
    int evaluatePattern(const GameBoard& board, int row, int col, int dRow, int dCol, Player player);
    int evaluateMove(const GameBoard& board, int row, int col, Player player);
    std::vector<std::pair<int, int>> getPossibleMoves(const GameBoard& board);
    Player getOpponent(Player player);
    std::pair<int, int> getRandomMove(const GameBoard& board);

    std::mt19937 rng;
    AIDifficulty currentDifficulty;
};

#endif // GAMEAI_H 