#include "ai/GameAI.h"
#include <algorithm>
#include <limits>
#include <random>
#include <chrono>
#include <iostream>

GameAI::GameAI(QObject* parent) : QObject(parent), currentDifficulty(AIDifficulty::MEDIUM) {
    std::random_device rd;
    rng.seed(rd());
}

void GameAI::setDifficulty(AIDifficulty difficulty) {
    currentDifficulty = difficulty;
}

AIDifficulty GameAI::getDifficulty() const {
    return currentDifficulty;
}

std::pair<int, int> GameAI::getBestMove(const GameBoard& board, Player player) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::cout << "\nAI is calculating move for " << (player == Player::BLACK ? "BLACK" : "WHITE") 
              << " player (Difficulty: " 
              << (currentDifficulty == AIDifficulty::EASY ? "EASY" : 
                  currentDifficulty == AIDifficulty::MEDIUM ? "MEDIUM" : "HARD")
              << ")..." << std::endl;
    
    std::pair<int, int> bestMove;
    
    switch (currentDifficulty) {
        case AIDifficulty::EASY:
            bestMove = getBestMoveWithDepth(board, player, 1);
            break;
        case AIDifficulty::MEDIUM:
            bestMove = getBestMoveWithDepth(board, player, 3);
            break;
        case AIDifficulty::HARD:
            bestMove = getBestMoveWithDepth(board, player, 5);
            break;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "move(" << bestMove.first << "," << bestMove.second 
              << ") (calculation took " << duration.count() << "ms)" << std::endl;
    
    return bestMove;
}

std::pair<int, int> GameAI::getBestMoveWithDepth(const GameBoard& board, Player player, int depth) {
    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();
    
    std::vector<std::pair<int, int>> moves = getPossibleMoves(board);
    std::sort(moves.begin(), moves.end(), [this, &board, player](const auto& a, const auto& b) {
        return evaluateMove(board, a.first, a.second, player) > 
               evaluateMove(board, b.first, b.second, player);
    });
    
    int bestScore = std::numeric_limits<int>::min();
    std::pair<int, int> bestMove = {-1, -1};
    
    GameBoard* tempBoard = new GameBoard();
    *tempBoard = board;
    
    for (const auto& move : moves) {
        if (tempBoard->makeMove(move.first, move.second, player)) {
            int score = -minimax(*tempBoard, depth - 1, -beta, -alpha, false, getOpponent(player));
            tempBoard->undoMove();
            
            std::cout << "Move (" << move.first << "," << move.second << ") score: " << score << std::endl;
            
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
            
            alpha = std::max(alpha, score);
            if (alpha >= beta) break;
        }
    }
    
    delete tempBoard;
    return bestMove;
}

std::pair<int, int> GameAI::getRandomMove(const GameBoard& board) {
    std::vector<std::pair<int, int>> moves = getPossibleMoves(board);
    if (moves.empty()) {
        return {-1, -1};
    }
    
    std::uniform_int_distribution<size_t> dist(0, moves.size() - 1);
    return moves[dist(rng)];
}

int GameAI::minimax(const GameBoard& board, int depth, int alpha, int beta, bool maximizing, Player player) {
    if (depth == 0 || board.getGameState() != GameState::IN_PROGRESS) {
        return evaluateBoard(board, player);
    }
    
    std::vector<std::pair<int, int>> moves = getPossibleMoves(board);
    std::sort(moves.begin(), moves.end(), [this, &board, player](const auto& a, const auto& b) {
        return evaluateMove(board, a.first, a.second, player) > 
               evaluateMove(board, b.first, b.second, player);
    });
    
    GameBoard* tempBoard = new GameBoard();
    *tempBoard = board;
    
    if (maximizing) {
        int maxScore = std::numeric_limits<int>::min();
        for (const auto& move : moves) {
            if (tempBoard->makeMove(move.first, move.second, player)) {
                int score = minimax(*tempBoard, depth - 1, alpha, beta, false, getOpponent(player));
                tempBoard->undoMove();
                maxScore = std::max(maxScore, score);
                alpha = std::max(alpha, score);
                if (alpha >= beta) {
                    delete tempBoard;
                    return maxScore; // Beta cutoff
                }
            }
        }
        delete tempBoard;
        return maxScore;
    } else {
        int minScore = std::numeric_limits<int>::max();
        for (const auto& move : moves) {
            if (tempBoard->makeMove(move.first, move.second, player)) {
                int score = minimax(*tempBoard, depth - 1, alpha, beta, true, getOpponent(player));
                tempBoard->undoMove();
                minScore = std::min(minScore, score);
                beta = std::min(beta, score);
                if (alpha >= beta) {
                    delete tempBoard;
                    return minScore; // Alpha cutoff
                }
            }
        }
        delete tempBoard;
        return minScore;
    }
}

int GameAI::evaluateBoard(const GameBoard& board, Player player) {
    int score = 0;
    Player opponent = getOpponent(player);
    
    int centerX = GameBoard::BOARD_SIZE / 2;
    int centerY = GameBoard::BOARD_SIZE / 2;
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            int x = centerX + i;
            int y = centerY + j;
            if (x >= 0 && x < GameBoard::BOARD_SIZE && y >= 0 && y < GameBoard::BOARD_SIZE) {
                if (board.getCell(x, y) == player) {
                    score += (3 - std::max(std::abs(i), std::abs(j))) * 10;
                }
            }
        }
    }
    
    for (int i = 0; i < GameBoard::BOARD_SIZE; ++i) {
        for (int j = 0; j < GameBoard::BOARD_SIZE; ++j) {
            if (board.getCell(i, j) == player) {
                for (int di = -1; di <= 1; ++di) {
                    for (int dj = -1; dj <= 1; ++dj) {
                        if (di == 0 && dj == 0) continue;
                        
                        score += evaluatePattern(board, i, j, di, dj, player);
                        score -= evaluatePattern(board, i, j, di, dj, opponent) * 2;
                    }
                }
            }
        }
    }
    
    return score;
}

int GameAI::evaluatePattern(const GameBoard& board, int row, int col, int dRow, int dCol, Player player) {
    int count = 1;
    int openEnds = 0;
    int spaces = 0;
    
    int r = row + dRow;
    int c = col + dCol;
    bool blocked = false;
    
    while (r >= 0 && r < GameBoard::BOARD_SIZE && c >= 0 && c < GameBoard::BOARD_SIZE && spaces <= 2) {
        if (board.getCell(r, c) == player) {
            if (spaces == 0) count++;
            else break;
        } else if (board.getCell(r, c) == Player::NONE) {
            spaces++;
            if (spaces == 1) openEnds++;
        } else {
            blocked = true;
            break;
        }
        r += dRow;
        c += dCol;
    }
    spaces = 0;
    
    r = row - dRow;
    c = col - dCol;
    
    while (r >= 0 && r < GameBoard::BOARD_SIZE && c >= 0 && c < GameBoard::BOARD_SIZE && spaces <= 2) {
        if (board.getCell(r, c) == player) {
            if (spaces == 0) count++;
            else break;
        } else if (board.getCell(r, c) == Player::NONE) {
            spaces++;
            if (spaces == 1) openEnds++;
        } else {
            blocked = true;
            break;
        }
        r -= dRow;
        c -= dCol;
    }
    
    int score = 0;
    if (count >= 6) return 100000;
    if (count == 5 && openEnds > 0) score = 50000;
    else if (count == 4 && openEnds == 2) score = 10000;
    else if (count == 4 && openEnds == 1) score = 5000;
    else if (count == 3 && openEnds == 2) score = 1000;
    else if (count == 3 && openEnds == 1) score = 500;
    else if (count == 2 && openEnds == 2) score = 100;
    else if (count == 2 && openEnds == 1) score = 50;
    
    if (blocked) score /= 2;
    
    return score;
}

int GameAI::evaluateMove(const GameBoard& board, int row, int col, Player player) {
    if (board.getCell(row, col) != Player::NONE) return std::numeric_limits<int>::min();
    
    int score = 0;
    
    int centerX = GameBoard::BOARD_SIZE / 2;
    int centerY = GameBoard::BOARD_SIZE / 2;
    int distanceFromCenter = std::max(std::abs(row - centerX), std::abs(col - centerY));
    score += (GameBoard::BOARD_SIZE / 2 - distanceFromCenter) * 10;
    
    for (int dRow = -1; dRow <= 1; ++dRow) {
        for (int dCol = -1; dCol <= 1; ++dCol) {
            if (dRow == 0 && dCol == 0) continue;
            
            GameBoard* tempBoard = new GameBoard();
            *tempBoard = board;
            tempBoard->makeMove(row, col, player);
            score += evaluatePattern(*tempBoard, row, col, dRow, dCol, player);
            
            Player opponent = getOpponent(player);
            score += evaluatePattern(*tempBoard, row, col, dRow, dCol, opponent) / 2;
            
            delete tempBoard;
        }
    }
    
    return score;
}

std::vector<std::pair<int, int>> GameAI::getPossibleMoves(const GameBoard& board) {
    std::vector<std::pair<int, int>> moves;
    
    for (int i = 0; i < GameBoard::BOARD_SIZE; ++i) {
        for (int j = 0; j < GameBoard::BOARD_SIZE; ++j) {
            if (board.getCell(i, j) == Player::NONE) {
                moves.emplace_back(i, j);
            }
        }
    }
    
    return moves;
}

Player GameAI::getOpponent(Player player) {
    return (player == Player::BLACK) ? Player::WHITE : Player::BLACK;
} 