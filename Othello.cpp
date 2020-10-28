#include <bangtal>
#include <iostream>
#include <string>

using namespace bangtal;
using namespace std;

ObjectPtr board[8][8];
ObjectPtr whiteScore[2], blackScore[2];
int maxX, maxY, maxNum;

enum class State {
    BLANK,
    POSSIBLE, //turn에 따라 달라짐
    BLACK,
    WHITE
};
State board_state[8][8]; //보드의 상태 저장

enum class Turn {
    BLACK,
    WHITE
};

Turn turn = Turn::BLACK; //turn의 값 black으로 초기화

void setState(int x, int y, State state) {
    switch (state) {
    case State::BLANK: board[y][x]->setImage("Images/blank.png"); break;
    case State::POSSIBLE: board[y][x]->setImage(turn == Turn::BLACK ? "Images/black possible.png" : "Images/white possible.png"); break;
    case State::BLACK: board[y][x]->setImage("Images/black.png"); break;
    case State::WHITE: board[y][x]->setImage("Images/white.png"); break;
    }
    board_state[y][x] = state;
}

bool checkPossible(int x, int y, int dx, int dy) {
    State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
    State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;
    int count = 0;
    int curX = x, curY = y;
    bool possible = false;
    for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
        if (board_state[y][x] == other) {
            count++;
            possible = true;
        }
        else if (board_state[y][x] == self) {
            if (possible && maxNum < count) {
                maxNum = count;
                maxY = curY;
                maxX = curX;
            }
            return possible;
        }
        else {
            return false;
        }


    }
    return false;
}

bool checkPossible(int x, int y) {
    if (board_state[y][x] == State::BLACK) return false;
    if (board_state[y][x] == State::WHITE) return false;
    setState(x, y, State::BLANK);

    int delta[8][2] = {
        {0,1},
        {1,1},
        {1,0},
        {1,-1},
        {0,-1},
        {-1,-1},
        {-1,0},
        {-1,1},
    };

    bool possible = false;
    for (auto d : delta) {
        if (checkPossible(x, y, d[0], d[1])) possible = true;

    }
    /*
    for (int i = 0; i < 8; i++) {
        checkPossible(x, y, delta[i][0], delta[i][1]);
    }
    */
    return possible; //8방향 중 하나라도 가능.
}

void reverse(int x, int y, int dx, int dy) {
    State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
    State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

    bool possible = false;
    for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
        if (board_state[y][x] == other) {
            possible = true;
        }
        else if (board_state[y][x] == self) {
            if (possible) {
                for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) {
                    if (board_state[y][x] == other) {
                        setState(x, y, self);
                    }
                    else {
                        return;
                    }
                }
            }
            else {
                return;
            }
            return;
        }
    }
}

void reverse(int x, int y) {
    int delta[8][2] = {
        {0,1},
        {1,1},
        {1,0},
        {1,-1},
        {0,-1},
        {-1,-1},
        {-1,0},
        {-1,1},
    };

    bool possible = false;
    for (auto d : delta) {
        reverse(x, y, d[0], d[1]);
    }
}


bool setPossible() {
    //8x8의 모든 위치에 대해서 현재 turn에 놓여질 수 있으면 상태를 POSSIBLE로 바꾼다.
    maxX = -1;
    maxY = -1;
    maxNum = -1;
    bool possible = false;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (checkPossible(x, y)) {
                setState(x, y, State::POSSIBLE);
                possible = true;
            }
        }
    }
    return possible;
}

void countScore() {
    int white = 0;
    int black = 0;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (board_state[y][x] == State::BLACK) black++;
            else if (board_state[y][x] == State::WHITE) white++;
        }
    }
    whiteScore[0]->setImage("Images/L" + to_string(white / 10) + ".png");
    whiteScore[1]->setImage("Images/L" + to_string(white % 10) + ".png");
    blackScore[0]->setImage("Images/L" + to_string(black / 10) + ".png");
    blackScore[1]->setImage("Images/L" + to_string(black % 10) + ".png");

}

void computerTurn() {
    if (maxNum <= 0) return;

    setState(maxX, maxY, State::WHITE);
    reverse(maxX, maxY);

}




int main()
{
    setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
    setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
    setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

    auto scene = Scene::create("Othello", "Images/background.png");

    whiteScore[0] = Object::create("Images/L0.png", scene, 1070, 220);
    whiteScore[1] = Object::create("Images/L2.png", scene, 1150, 220);
    blackScore[0] = Object::create("Images/L0.png", scene, 750, 220);
    blackScore[1] = Object::create("Images/L2.png", scene, 830, 220);


    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            board[y][x] = Object::create("Images/blank.png", scene, 40 + x * 80, 40 + y * 80);
            board[y][x]->setOnMouseCallback([&, x, y](ObjectPtr object, int, int, MouseAction)->bool {
                if (board_state[y][x] == State::POSSIBLE) {
                    if (turn == Turn::BLACK) {
                        setState(x, y, State::BLACK);
                        reverse(x, y);
                        turn = Turn::WHITE;
                    }
                    countScore();
                    if (!setPossible()) {
                        turn = Turn::BLACK;
                        if (!setPossible()) {
                            showMessage("End Game!!!");
                            return true;
                        }
                        else return true;
                    }
                    do {
                        computerTurn();
                        countScore();
                        turn = Turn::BLACK;
                        if (!setPossible()) {
                            turn = Turn::WHITE;

                            if (!setPossible()) {
                                showMessage("End Game!!!");
                                return true;
                            }
                        }
                        else break;

                    } while (true);

                }

                return true;
            });
            board_state[y][x] = State::BLANK;
        }
    }

    setState(3, 3, State::BLACK);
    setState(4, 4, State::BLACK);
    setState(3, 4, State::WHITE);
    setState(4, 3, State::WHITE);

    setPossible();

    startGame(scene);
}
