#ifndef MAINMENUWIDGET_H
#define MAINMENUWIDGET_H

#include <QWidget>
#include "core/Player.h"

class QPushButton;
class QComboBox;
class QVBoxLayout;

class MainMenuWidget : public QWidget {
    Q_OBJECT

public:
    explicit MainMenuWidget(QWidget* parent = nullptr);

signals:
    void startGameClicked(int gameMode, int difficulty, bool playAsBlack);
    void exitClicked();

private slots:
    void onStartGameClicked();

private:
    QPushButton* startGameButton;
    QPushButton* exitButton;
    QComboBox* gameModeCombo;
    QComboBox* difficultyCombo;
    QComboBox* colorCombo;
    QVBoxLayout* layout;
};

#endif // MAINMENUWIDGET_H 