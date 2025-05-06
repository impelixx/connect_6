#include "gui/GameBoardWidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QLinearGradient>

GameBoardWidget::GameBoardWidget(GameBoard* board, QWidget* parent)
    : QWidget(parent)
    , board(board)
    , ai(new GameAI())
    , ai2(new GameAI())
    , gameMode(0)
    , humanPlayer(Player::BLACK)
    , hintCell(nullptr) {
    setupUI();
    setupConnections();
    updateBoard();
}

GameBoardWidget::~GameBoardWidget() {
    delete ai;
    delete ai2;
}

void GameBoardWidget::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Set minimum size and scaling policy
    setMinimumSize(600, 650);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Game info panel
    QWidget* infoPanel = new QWidget();
    infoPanel->setStyleSheet("background-color: #2c3e50; border-radius: 4px; padding: 4px;");
    QHBoxLayout* infoLayout = new QHBoxLayout(infoPanel);
    infoLayout->setSpacing(10);
    infoLayout->setContentsMargins(5, 5, 5, 5);

    gameModeLabel = new QLabel();
    gameModeLabel->setStyleSheet("color: white; font-size: 11px; font-weight: bold;");
    infoLayout->addWidget(gameModeLabel);

    difficultyLabel = new QLabel();
    difficultyLabel->setStyleSheet("color: white; font-size: 11px; font-weight: bold;");
    infoLayout->addWidget(difficultyLabel);

    currentPlayerLabel = new QLabel();
    currentPlayerLabel->setStyleSheet("color: white; font-size: 11px; font-weight: bold;");
    infoLayout->addWidget(currentPlayerLabel);

    infoLayout->addStretch();
    mainLayout->addWidget(infoPanel);

    // Control panel with improved styling
    QWidget* controlPanel = new QWidget();
    controlPanel->setStyleSheet("background-color: #34495e; border-radius: 4px; padding: 4px;");
    controlLayout = new QHBoxLayout(controlPanel);
    controlLayout->setSpacing(5);
    controlLayout->setContentsMargins(5, 5, 5, 5);

    QString buttonStyle = "QPushButton {"
                         "    padding: 4px 8px;"
                         "    border-radius: 3px;"
                         "    background-color: #3498db;"
                         "    color: white;"
                         "    border: none;"
                         "    font-size: 11px;"
                         "    font-weight: bold;"
                         "    min-width: 60px;"
                         "}"
                         "QPushButton:hover {"
                         "    background-color: #2980b9;"
                         "}"
                         "QPushButton:pressed {"
                         "    background-color: #1c6ea4;"
                         "}"
                         "QPushButton:checked {"
                         "    background-color: #e74c3c;"
                         "}";

    QString comboBoxStyle = "QComboBox {"
                           "    padding: 4px 8px;"
                           "    border-radius: 3px;"
                           "    background-color: #ffffff;"
                           "    min-width: 100px;"
                           "    font-size: 12px;"
                           "    border: 1px solid #bdc3c7;"
                           "    color: #2c3e50;"
                           "}"
                           "QComboBox::drop-down {"
                           "    border: none;"
                           "    width: 20px;"
                           "}"
                           "QComboBox::down-arrow {"
                           "    image: none;"
                           "    border-left: 5px solid transparent;"
                           "    border-right: 5px solid transparent;"
                           "    border-top: 5px solid #2c3e50;"
                           "    margin-right: 5px;"
                           "}"
                           "QComboBox:hover {"
                           "    background-color: #f8f9fa;"
                           "    border: 1px solid #95a5a6;"
                           "}"
                           "QComboBox:focus {"
                           "    border: 1px solid #3498db;"
                           "    background-color: #ffffff;"
                           "}"
                           "QComboBox::item {"
                           "    color: white;"
                           "    background-color: #2c3e50"
                           "}"
                           "QComboBox::item:selected {"
                           "    background-color: #3498db;"
                           "    color: #ffffff;"
                           "}"
                           "QComboBox::item:hover {"
                           "    background-color: #e8f6ff;"
                           "    color: #2c3e50;"
                           "}";

    QWidget* difficultyContainer = new QWidget();
    QHBoxLayout* difficultyLayout = new QHBoxLayout(difficultyContainer);
    difficultyLayout->setSpacing(5);
    difficultyLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* blackAILabel = new QLabel("Black AI:");
    blackAILabel->setStyleSheet("color: white; font-size: 11px; font-weight: bold;");
    difficultyLayout->addWidget(blackAILabel);

    difficultyComboBox = new QComboBox();
    difficultyComboBox->addItem("Easy", static_cast<int>(AIDifficulty::EASY));
    difficultyComboBox->addItem("Medium", static_cast<int>(AIDifficulty::MEDIUM));
    difficultyComboBox->addItem("Hard", static_cast<int>(AIDifficulty::HARD));
    difficultyComboBox->setCurrentIndex(1);
    difficultyComboBox->setStyleSheet(comboBoxStyle);
    difficultyLayout->addWidget(difficultyComboBox);

    QLabel* redAILabel = new QLabel("Red AI:");
    redAILabel->setStyleSheet("color: white; font-size: 11px; font-weight: bold;");
    redAILabel->hide();
    difficultyLayout->addWidget(redAILabel);

    ai2DifficultyComboBox = new QComboBox();
    ai2DifficultyComboBox->addItem("Easy", static_cast<int>(AIDifficulty::EASY));
    ai2DifficultyComboBox->addItem("Medium", static_cast<int>(AIDifficulty::MEDIUM));
    ai2DifficultyComboBox->addItem("Hard", static_cast<int>(AIDifficulty::HARD));
    ai2DifficultyComboBox->setCurrentIndex(1);
    ai2DifficultyComboBox->setStyleSheet(comboBoxStyle);
    ai2DifficultyComboBox->hide();
    difficultyLayout->addWidget(ai2DifficultyComboBox);

    difficultyLayout->addStretch();
    controlLayout->addWidget(difficultyContainer);

    undoButton = new QPushButton("Undo");
    hintButton = new QPushButton("Hint");
    saveButton = new QPushButton("Save");
    loadButton = new QPushButton("Load");
    resetButton = new QPushButton("Reset");
    menuButton = new QPushButton("Menu");
    pauseButton = new QPushButton("Pause");
    pauseButton->setCheckable(true);

    undoButton->setStyleSheet(buttonStyle);
    hintButton->setStyleSheet(buttonStyle);
    saveButton->setStyleSheet(buttonStyle);
    loadButton->setStyleSheet(buttonStyle);
    resetButton->setStyleSheet(buttonStyle);
    menuButton->setStyleSheet(buttonStyle);
    pauseButton->setStyleSheet(buttonStyle);

    controlLayout->addWidget(undoButton);
    controlLayout->addWidget(hintButton);
    controlLayout->addWidget(saveButton);
    controlLayout->addWidget(loadButton);
    controlLayout->addWidget(resetButton);
    controlLayout->addWidget(pauseButton);
    controlLayout->addWidget(menuButton);

    mainLayout->addWidget(controlPanel);

    QWidget* boardContainer = new QWidget();
    boardContainer->setStyleSheet("background-color: #f5d76e; border-radius: 4px; padding: 4px;");
    boardContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout* containerLayout = new QVBoxLayout(boardContainer);
    containerLayout->setSpacing(0);
    containerLayout->setContentsMargins(2, 2, 2, 2);

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(2);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < 15; ++i) {
        gridLayout->setRowStretch(i, 1);
        gridLayout->setColumnStretch(i, 1);
    }

    for (int row = 0; row < 15; ++row) {
        for (int col = 0; col < 15; ++col) {
            QPushButton* cell = new QPushButton();
            cell->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            cell->setMinimumSize(35, 35);
            cell->setProperty("row", row);
            cell->setProperty("col", col);
            
            QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
            shadow->setBlurRadius(2);
            shadow->setColor(QColor(0, 0, 0, 60));
            shadow->setOffset(0.5, 0.5);
            cell->setGraphicsEffect(shadow);

            cell->setStyleSheet("QPushButton {"
                               "    background-color: #f0f0f0;"
                               "    border: 1px solid #ccc;"
                               "    border-radius: 50%;"
                               "}"
                               "QPushButton:hover {"
                               "    background-color: #e0e0e0;"
                               "}");

            gridLayout->addWidget(cell, row, col);
        }
    }

    containerLayout->addLayout(gridLayout);
    mainLayout->addWidget(boardContainer, 1);

    setStyleSheet("QWidget {"
                  "    background-color: #ecf0f1;"
                  "}");
}

void GameBoardWidget::setupConnections() {
    connect(difficultyComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GameBoardWidget::onDifficultyChanged);
    connect(ai2DifficultyComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GameBoardWidget::onAIDifficultyChanged);

    for (int row = 0; row < GameBoard::BOARD_SIZE; ++row) {
        for (int col = 0; col < GameBoard::BOARD_SIZE; ++col) {
            QPushButton* cell = qobject_cast<QPushButton*>(gridLayout->itemAtPosition(row, col)->widget());
            connect(cell, &QPushButton::clicked, this, [this, row, col]() { handleCellClicked(row, col); });
        }
    }

    connect(undoButton, &QPushButton::clicked, this, &GameBoardWidget::undoMove);
    connect(hintButton, &QPushButton::clicked, this, &GameBoardWidget::getHint);
    connect(saveButton, &QPushButton::clicked, this, &GameBoardWidget::saveGame);
    connect(loadButton, &QPushButton::clicked, this, &GameBoardWidget::loadGame);
    connect(resetButton, &QPushButton::clicked, this, &GameBoardWidget::resetGame);
    connect(menuButton, &QPushButton::clicked, this, &GameBoardWidget::onMenuClicked);
    connect(pauseButton, &QPushButton::clicked, this, &GameBoardWidget::onPauseClicked);
}

void GameBoardWidget::onDifficultyChanged(int index) {
    AIDifficulty difficulty = static_cast<AIDifficulty>(difficultyComboBox->itemData(index).toInt());
    ai->setDifficulty(difficulty);
    updateStatus();
}

void GameBoardWidget::onAIDifficultyChanged(int index) {
    AIDifficulty difficulty = static_cast<AIDifficulty>(ai2DifficultyComboBox->itemData(index).toInt());
    ai2->setDifficulty(difficulty);
    updateStatus();
}

void GameBoardWidget::setAIDifficulty(AIDifficulty difficulty) {
    int index = difficultyComboBox->findData(static_cast<int>(difficulty));
    if (index != -1) {
        difficultyComboBox->setCurrentIndex(index);
    }
}

void GameBoardWidget::updateBoard() {
    for (int row = 0; row < 15; ++row) {
        for (int col = 0; col < 15; ++col) {
            QPushButton* cell = qobject_cast<QPushButton*>(gridLayout->itemAtPosition(row, col)->widget());
            Player player = board->getCell(row, col);
            
            QString style = "QPushButton {"
                          "    border: 1px solid #ccc;"
                          "    border-radius: 50%;"
                          "}";
            
            if (player == Player::BLACK) {
                style += "QPushButton {"
                        "    background-color: #2c3e50;"
                        "    border: 1px solid #1a252f;"
                        "}";
            } else if (player == Player::WHITE) {
                style += "QPushButton {"
                        "    background-color: #e74c3c;"
                        "    border: 1px solid #c0392b;"
                        "}";
            } else {
                style += "QPushButton {"
                        "    background-color: #f0f0f0;"
                        "}";
            }
            
            cell->setStyleSheet(style);
        }
    }
    updateStatus();
}

void GameBoardWidget::handleCellClicked(int row, int col) {
    if (board->getGameState() != GameState::IN_PROGRESS) return;
    
    Player current = board->getCurrentPlayer();
    if (gameMode == 1 && current != humanPlayer) return;
    
    if (board->makeMove(row, col, current)) {
        QPushButton* cell = qobject_cast<QPushButton*>(gridLayout->itemAtPosition(row, col)->widget());
        QPropertyAnimation* animation = new QPropertyAnimation(cell, "geometry");
        animation->setDuration(300);
        animation->setStartValue(QRect(cell->x(), -30, cell->width(), cell->height()));
        animation->setEndValue(QRect(cell->x(), cell->y(), cell->width(), cell->height()));
        animation->setEasingCurve(QEasingCurve::OutBounce);
        
        connect(animation, &QPropertyAnimation::finished, this, [this]() {
            updateBoard();
            
            if (board->getGameState() != GameState::IN_PROGRESS) {
                highlightWinningLine();
            }
            
            if (board->getGameState() == GameState::IN_PROGRESS && gameMode == 1 && board->getCurrentPlayer() != humanPlayer) {
                QTimer::singleShot(1000, this, &GameBoardWidget::makeAIMove);
            }
        });
        
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void GameBoardWidget::undoMove() {
    board->undoLastMove();
    updateBoard();
}

void GameBoardWidget::getHint() {
    if (board->getGameState() != GameState::IN_PROGRESS || gameMode == 2) return;
    
    Player current = board->getCurrentPlayer();
    if (gameMode == 1 && current != humanPlayer) return;
    
    AIDifficulty currentDifficulty = ai->getDifficulty();
    ai->setDifficulty(AIDifficulty::HARD);
    auto [row, col] = ai->getBestMove(*board, current);
    ai->setDifficulty(currentDifficulty);
    
    if (hintCell) {
        QString currentStyle = hintCell->styleSheet();
        currentStyle = currentStyle.replace("border: 3px solid #3498db;", "");
        currentStyle = currentStyle.replace("background-color: #e3f2fd;", "");
        currentStyle = currentStyle.replace("box-shadow: 0 0 10px #3498db;", "");
        hintCell->setStyleSheet(currentStyle);
        
        QGraphicsDropShadowEffect* oldEffect = qobject_cast<QGraphicsDropShadowEffect*>(hintCell->graphicsEffect());
        if (oldEffect) {
            hintCell->setGraphicsEffect(nullptr);
            delete oldEffect;
        }
    }
    
    hintCell = qobject_cast<QPushButton*>(gridLayout->itemAtPosition(row, col)->widget());
    
    QGraphicsDropShadowEffect* glowEffect = new QGraphicsDropShadowEffect();
    glowEffect->setBlurRadius(20);
    glowEffect->setColor(QColor(52, 152, 219, 255));
    glowEffect->setOffset(0, 0);
    hintCell->setGraphicsEffect(glowEffect);
    
    QString style = hintCell->styleSheet();
    style += "border: 4px solid #3498db;"
             "background-color: #bbdefb !important;"
             "box-shadow: 0 0 15px #3498db;";
    hintCell->setStyleSheet(style);
    
    QPropertyAnimation* animation = new QPropertyAnimation(hintCell, "geometry");
    animation->setDuration(300);
    animation->setStartValue(QRect(hintCell->x(), hintCell->y() - 10, hintCell->width(), hintCell->height()));
    animation->setEndValue(QRect(hintCell->x(), hintCell->y(), hintCell->width(), hintCell->height()));
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    
    QPropertyAnimation* pulseAnimation = new QPropertyAnimation(glowEffect, "blurRadius");
    pulseAnimation->setDuration(1000);
    pulseAnimation->setStartValue(20);
    pulseAnimation->setEndValue(30);
    pulseAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    pulseAnimation->setLoopCount(-1);
    pulseAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void GameBoardWidget::saveGame() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Game", "", "Game Files (*.game)");
    if (!fileName.isEmpty()) {
        board->saveGame(fileName.toStdString());
    }
}

void GameBoardWidget::loadGame() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load Game", "", "Game Files (*.game)");
    if (!fileName.isEmpty()) {
        if (board->loadGame(fileName.toStdString())) {
            updateBoard();
        } else {
            QMessageBox::warning(this, "Error", "Failed to load game");
        }
    }
}

void GameBoardWidget::resetGame() {
    QList<QPropertyAnimation*> animations = findChildren<QPropertyAnimation*>();
    for (QPropertyAnimation* animation : animations) {
        animation->stop();
        animation->deleteLater();
    }

    board->reset();
    for (int row = 0; row < GameBoard::BOARD_SIZE; ++row) {
        for (int col = 0; col < GameBoard::BOARD_SIZE; ++col) {
            QPushButton* cell = qobject_cast<QPushButton*>(gridLayout->itemAtPosition(row, col)->widget());
            if (cell) {
                QGraphicsEffect* effect = cell->graphicsEffect();
                if (effect) {
                    cell->setGraphicsEffect(nullptr);
                    delete effect;
                }
            }
        }
    }
    
    updateBoard();
    
    if (gameMode == 2) {
        QTimer::singleShot(500, this, &GameBoardWidget::makeAIMove);
    }
}

void GameBoardWidget::updateStatus() {
    QString gameModeText;
    switch (gameMode) {
        case 0:
            gameModeText = "Game Mode: Human vs Human";
            break;
        case 1:
            gameModeText = "Game Mode: Human vs AI";
            break;
        case 2:
            gameModeText = "Game Mode: AI vs AI";
            break;
    }
    gameModeLabel->setText(gameModeText);

    QString difficultyText;
    if (gameMode == 2) {
        difficultyText = "Black AI: " + difficultyComboBox->currentText() + 
                        " | Red AI: " + ai2DifficultyComboBox->currentText();
    } else {
        difficultyText = "Difficulty: " + difficultyComboBox->currentText();
    }
    difficultyLabel->setText(difficultyText);

    QString currentPlayerText;
    if (board->getGameState() == GameState::IN_PROGRESS) {
        currentPlayerText = "Current Player: " + QString(board->getCurrentPlayer() == Player::BLACK ? "Black" : "Red");
    } else {
        switch (board->getGameState()) {
            case GameState::BLACK_WIN:
                currentPlayerText = "Black Wins!";
                break;
            case GameState::WHITE_WIN:
                currentPlayerText = "Red Wins!";
                break;
            case GameState::DRAW:
                currentPlayerText = "It's a Draw!";
                break;
            default:
                currentPlayerText = "Game Over";
                break;
        }
    }
    currentPlayerLabel->setText(currentPlayerText);
}

void GameBoardWidget::makeAIMove() {
    if (board->getGameState() != GameState::IN_PROGRESS || pauseButton->isChecked()) return;
    
    Player current = board->getCurrentPlayer();
    if (gameMode == 1 && current == humanPlayer) return;
    
    GameAI* currentAI = (gameMode == 2 && current == Player::WHITE) ? ai2 : ai;
    auto [row, col] = currentAI->getBestMove(*board, current);
    
    if (board->makeMove(row, col, current)) {
        QPushButton* cell = qobject_cast<QPushButton*>(gridLayout->itemAtPosition(row, col)->widget());
        QPropertyAnimation* animation = new QPropertyAnimation(cell, "geometry");
        animation->setDuration(300);
        animation->setStartValue(QRect(cell->x(), -30, cell->width(), cell->height()));
        animation->setEndValue(QRect(cell->x(), cell->y(), cell->width(), cell->height()));
        animation->setEasingCurve(QEasingCurve::OutBounce);
        
        connect(animation, &QPropertyAnimation::finished, this, [this]() {
            updateBoard();
            
            if (board->getGameState() != GameState::IN_PROGRESS) {
                highlightWinningLine();
            }
            
            if (board->getGameState() == GameState::IN_PROGRESS && gameMode == 2 && !pauseButton->isChecked()) {
                QTimer::singleShot(1000, this, &GameBoardWidget::makeAIMove);
            }
        });
        
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void GameBoardWidget::highlightWinningLine() {
    auto winningLine = board->getWinningLine();
    if (winningLine.empty()) return;

    QString winningStyle = "QPushButton {"
                          "    border: 3px solid #27ae60;"
                          "    background-color: %1;"
                          "    border-radius: 50%;"
                          "}"
                          "QPushButton:hover {"
                          "    background-color: %1;"
                          "}";

    for (const auto& pos : winningLine) {
        QPushButton* cell = qobject_cast<QPushButton*>(gridLayout->itemAtPosition(pos.first, pos.second)->widget());
        if (!cell) continue;

        QGraphicsDropShadowEffect* glowEffect = new QGraphicsDropShadowEffect();
        glowEffect->setBlurRadius(10);
        glowEffect->setColor(QColor(46, 204, 113, 150));
        glowEffect->setOffset(0, 0);
        cell->setGraphicsEffect(glowEffect);

        Player cellPlayer = board->getCell(pos.first, pos.second);
        QString color = (cellPlayer == Player::BLACK) ? "#2c3e50" : "#e74c3c";
        cell->setStyleSheet(winningStyle.arg(color));
    }
}

void GameBoardWidget::setGameMode(int mode) {
    gameMode = mode;
    if (mode == 2) {
        QLabel* blackLabel = findChild<QLabel*>("Black AI:");
        QLabel* redLabel = findChild<QLabel*>("Red AI:");
        if (blackLabel) blackLabel->show();
        if (redLabel) redLabel->show();
        difficultyComboBox->show();
        ai2DifficultyComboBox->show();
        ai2DifficultyComboBox->setCurrentIndex(difficultyComboBox->currentIndex());
        board->reset();
        updateBoard();
        QTimer::singleShot(500, this, &GameBoardWidget::makeAIMove);
    } else {
        QLabel* blackLabel = findChild<QLabel*>("Black AI:");
        QLabel* redLabel = findChild<QLabel*>("Red AI:");
        if (blackLabel) blackLabel->setText("Difficulty:");
        if (redLabel) redLabel->hide();
        difficultyComboBox->show();
        ai2DifficultyComboBox->hide();
        board->reset();
        updateBoard();
    }
    updateStatus();
}

void GameBoardWidget::setHumanPlayer(Player player) {
    humanPlayer = player;
    updateStatus();
}

void GameBoardWidget::onMenuClicked() {
    board->reset();
    emit returnToMainMenu();
}

void GameBoardWidget::onPauseClicked() {
    if (!pauseButton->isChecked() && gameMode == 2 && board->getGameState() == GameState::IN_PROGRESS) {
        QTimer::singleShot(500, this, &GameBoardWidget::makeAIMove);
    }
} 