#include "gui/MainWindow.h"
#include "gui/MainMenuWidget.h"
#include <QStackedWidget>
#include <QVBoxLayout>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , stackedWidget(new QStackedWidget(this))
    , mainMenu(new MainMenuWidget(this))
    , gameBoard(new GameBoard)
    , gameBoardWidget(new GameBoardWidget(gameBoard, this))
{
    setCentralWidget(stackedWidget);
    
    stackedWidget->addWidget(mainMenu);
    stackedWidget->addWidget(gameBoardWidget);
    
    stackedWidget->setCurrentWidget(mainMenu);
    
    connect(mainMenu, &MainMenuWidget::startGameClicked,
            this, &MainWindow::startGame);
    connect(mainMenu, &MainMenuWidget::exitClicked,
            this, &QMainWindow::close);
    connect(gameBoardWidget, &GameBoardWidget::returnToMainMenu,
            this, &MainWindow::showMainMenu);
    
    // Set fixed window size
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setWindowTitle(tr("Connect 6"));
}

MainWindow::~MainWindow() {
    delete gameBoard;
}

void MainWindow::startGame(int gameMode, int difficulty, bool playAsBlack) {
    gameBoardWidget->setAIDifficulty(static_cast<AIDifficulty>(difficulty));
    gameBoardWidget->setGameMode(gameMode);
    stackedWidget->setCurrentWidget(gameBoardWidget);
}

void MainWindow::showMainMenu() {
    gameBoard->reset();
    stackedWidget->setCurrentWidget(mainMenu);
} 