#include "gui/MainMenuWidget.h"
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>

MainMenuWidget::MainMenuWidget(QWidget* parent)
    : QWidget(parent)
    , startGameButton(new QPushButton(tr("Start Game"), this))
    , exitButton(new QPushButton(tr("Exit"), this))
    , gameModeCombo(new QComboBox(this))
    , difficultyCombo(new QComboBox(this))
    , colorCombo(new QComboBox(this))
    , layout(new QVBoxLayout(this))
{
    // Setup game mode combo
    gameModeCombo->addItem(tr("Player vs Player"), 0);
    gameModeCombo->addItem(tr("Player vs AI"), 1);
    gameModeCombo->addItem(tr("AI vs AI"), 2);

    // Setup difficulty combo
    difficultyCombo->addItem(tr("Easy"), 0);
    difficultyCombo->addItem(tr("Medium"), 1);
    difficultyCombo->addItem(tr("Hard"), 2);
    difficultyCombo->setCurrentIndex(1); // Default to Medium

    // Setup color combo
    colorCombo->addItem(tr("Play as Black"), true);
    colorCombo->addItem(tr("Play as White"), false);

    // Style buttons
    startGameButton->setMinimumHeight(40);
    exitButton->setMinimumHeight(40);
    startGameButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
    );
    exitButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #f44336;"
        "    color: white;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #da190b;"
        "}"
    );

    QLabel* titleLabel = new QLabel(tr("Connect 6"), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 32px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "    margin: 20px;"
        "}"
    );

    QLabel* gameModeLabel = new QLabel(tr("Game Mode:"), this);
    QLabel* difficultyLabel = new QLabel(tr("AI Difficulty:"), this);
    QLabel* colorLabel = new QLabel(tr("Player Color:"), this);

    // Setup layout
    layout->addWidget(titleLabel);
    layout->addSpacing(20);
    layout->addWidget(gameModeLabel);
    layout->addWidget(gameModeCombo);
    layout->addSpacing(10);
    layout->addWidget(difficultyLabel);
    layout->addWidget(difficultyCombo);
    layout->addSpacing(10);
    layout->addWidget(colorLabel);
    layout->addWidget(colorCombo);
    layout->addSpacing(20);
    layout->addWidget(startGameButton);
    layout->addWidget(exitButton);
    layout->addStretch();
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(10);

    // Connect signals
    connect(startGameButton, &QPushButton::clicked, this, &MainMenuWidget::onStartGameClicked);
    connect(exitButton, &QPushButton::clicked, this, &MainMenuWidget::exitClicked);
    
    QString comboStyle = 
        "QComboBox {"
        "    padding: 8px;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    min-height: 30px;"
        "    background-color: white;"
        "    font-size: 14px;"
        "}"
        "QComboBox:hover {"
        "    border-color: #3498db;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "    width: 30px;"
        "}"
        "QComboBox::down-arrow {"
        "    image: none;"
        "    border-left: 5px solid transparent;"
        "    border-right: 5px solid transparent;"
        "    border-top: 7px solid #34495e;"
        "    margin-right: 8px;"
        "}"
        "QComboBox:on {"
        "    border-color: #3498db;"
        "    background-color: #f8f9fa;"
        "}"
        "QComboBox QAbstractItemView {"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    background-color: white;"
        "    selection-background-color:rgb(0, 66, 110);"
        "    selection-color: black;"
        "}";

    QString labelStyle =
        "QLabel {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    color: #34495e;"
        "    margin-bottom: 5px;"
        "}";

    setStyleSheet(comboStyle + labelStyle);
    colorCombo->setVisible(false);
    connect(gameModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [difficultyLabel, colorLabel, this](int index)
            {
                bool showDifficulty = (index == 1 || index == 2);
                difficultyLabel->setVisible(showDifficulty);
                difficultyCombo->setVisible(showDifficulty);
                colorLabel->setVisible(index == 0);
                // colorCombo->setVisible(index == 0);
            });

    gameModeCombo->currentIndexChanged(gameModeCombo->currentIndex());
}

void MainMenuWidget::onStartGameClicked() {
    int gameMode = gameModeCombo->currentData().toInt();
    int difficulty = difficultyCombo->isVisible() ? difficultyCombo->currentData().toInt() : 1;
    bool playAsBlack = gameMode == 0 ? colorCombo->currentData().toBool() : true;
    emit startGameClicked(gameMode, difficulty, playAsBlack);
} 