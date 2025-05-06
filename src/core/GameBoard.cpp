#include "core/GameBoard.h"
#include <fstream>
#include <sstream>
#include <algorithm>

GameBoard::GameBoard() : currentPlayer(Player::BLACK), gameState(GameState::IN_PROGRESS) {
    board.resize(BOARD_SIZE, std::vector<Player>(BOARD_SIZE, Player::NONE));
}

GameBoard& GameBoard::operator=(const GameBoard& other) {
    if (this != &other) {
        board = other.board;
        moveHistory = other.moveHistory;
        currentPlayer = other.currentPlayer;
        gameState = other.gameState;
    }
    return *this;
}

bool GameBoard::makeMove(int row, int col, Player player) {
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE ||
        board[row][col] != Player::NONE || gameState != GameState::IN_PROGRESS ||
        player != currentPlayer) {
        return false;
    }

    board[row][col] = player;
    moveHistory.emplace_back(row, col, player);

    if (checkWin(row, col, player)) {
        gameState = (player == Player::BLACK) ? GameState::BLACK_WIN : GameState::WHITE_WIN;
    } else if (moveHistory.size() == BOARD_SIZE * BOARD_SIZE) {
        gameState = GameState::DRAW;
    }

    currentPlayer = (player == Player::BLACK) ? Player::WHITE : Player::BLACK;
    return true;
}

bool GameBoard::isValidMove(int row, int col) const {
    return row >= 0 && row < BOARD_SIZE && 
           col >= 0 && col < BOARD_SIZE && 
           board[row][col] == Player::NONE;
}

GameState GameBoard::getGameState() const {
    return gameState;
}

Player GameBoard::getCurrentPlayer() const {
    return currentPlayer;
}

Player GameBoard::getCell(int row, int col) const {
    return board[row][col];
}

void GameBoard::reset() {
    board = std::vector<std::vector<Player>>(BOARD_SIZE, std::vector<Player>(BOARD_SIZE, Player::NONE));
    moveHistory.clear();
    winningLine.clear();
    currentPlayer = Player::BLACK;
    gameState = GameState::IN_PROGRESS;
    emit boardChanged();
    emit gameStateChanged(gameState);
}

void GameBoard::undoLastMove() {
    if (moveHistory.empty()) return;

    auto [row, col, player] = moveHistory.back();
    board[row][col] = Player::NONE;
    moveHistory.pop_back();
    currentPlayer = player;
    gameState = GameState::IN_PROGRESS;
    emit boardChanged();
}

void GameBoard::undoMove() {
    if (!moveHistory.empty()) {
        auto [row, col, player] = moveHistory.back();
        board[row][col] = Player::NONE;
        moveHistory.pop_back();
        currentPlayer = player;
        gameState = GameState::IN_PROGRESS;
    }
}

std::vector<std::pair<int, int>> GameBoard::getValidMoves() const {
    std::vector<std::pair<int, int>> moves;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == Player::NONE) {
                moves.emplace_back(i, j);
            }
        }
    }
    return moves;
}

std::string GameBoard::getMoveHistory() const {
    std::stringstream ss;
    for (const auto& [row, col, player] : moveHistory) {
        ss << (player == Player::BLACK ? "B" : "W") << " " << row << " " << col << "\n";
    }
    return ss.str();
}

void GameBoard::saveGame(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) return;

    file << getMoveHistory();
}

bool GameBoard::loadGame(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return false;

    reset();
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        char player;
        int row, col;
        if (ss >> player >> row >> col) {
            Player p = (player == 'B') ? Player::BLACK : Player::WHITE;
            if (!makeMove(row, col, p)) {
                reset();
                return false;
            }
        }
    }
    return true;
}

std::pair<int, int> GameBoard::getHint() const {
    // Simple hint implementation - returns the center of the board if empty,
    // otherwise returns a random valid move
    if (board[BOARD_SIZE/2][BOARD_SIZE/2] == Player::NONE) {
        return {BOARD_SIZE/2, BOARD_SIZE/2};
    }

    auto validMoves = getValidMoves();
    if (validMoves.empty()) return {-1, -1};
    return validMoves[0]; // In a real implementation, this would use AI to find the best move
}

bool GameBoard::checkWin(int row, int col, Player player) {
    // Check horizontal
    if (checkLine(row, col, 0, 1, player)) return true;
    // Check vertical
    if (checkLine(row, col, 1, 0, player)) return true;
    // Check diagonal
    if (checkLine(row, col, 1, 1, player)) return true;
    // Check other diagonal
    if (checkLine(row, col, 1, -1, player)) return true;
    return false;
}

bool GameBoard::checkLine(int row, int col, int dRow, int dCol, Player player) {
    int count = 1;
    winningLine.clear();
    winningLine.push_back({row, col});

    // Check forward
    int r = row + dRow;
    int c = col + dCol;
    while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && board[r][c] == player) {
        count++;
        winningLine.push_back({r, c});
        r += dRow;
        c += dCol;
    }

    // Check backward
    r = row - dRow;
    c = col - dCol;
    while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && board[r][c] == player) {
        count++;
        winningLine.push_back({r, c});
        r -= dRow;
        c -= dCol;
    }

    if (count >= WIN_LENGTH) {
        return true;
    }
    
    winningLine.clear();
    return false;
}

bool GameBoard::checkDirection(int row, int col, int dRow, int dCol, Player player) const {
    int count = 1;
    // Check in positive direction
    for (int i = 1; i < WIN_LENGTH; ++i) {
        int newRow = row + i * dRow;
        int newCol = col + i * dCol;
        if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE ||
            board[newRow][newCol] != player) {
            break;
        }
        count++;
    }
    // Check in negative direction
    for (int i = 1; i < WIN_LENGTH; ++i) {
        int newRow = row - i * dRow;
        int newCol = col - i * dCol;
        if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE ||
            board[newRow][newCol] != player) {
            break;
        }
        count++;
    }
    return count >= WIN_LENGTH;
}

std::vector<std::pair<int, int>> GameBoard::getWinningLine() const {
    return winningLine;
} 