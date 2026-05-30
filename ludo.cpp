#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <random>
#include <limits>
#include <algorithm>

using namespace std;

class LudoGame {
private:
    static constexpr int BOARD_SIZE = 7;
    static constexpr int TOKEN_COUNT = 4;
    static constexpr int PATH_LENGTH = 24;
    static constexpr int HOME = -1;
    static constexpr int FINISHED = PATH_LENGTH;

    struct Coord {
        int row{};
        int col{};
    };

    struct Token {
        int progress = HOME;   // HOME = -1, 0..23 = on path, 24 = finished
    };

    struct Player {
        string name;
        char symbol{};
        int startOffset{};
        vector<Token> tokens;
    };

    enum class ActionType { EnterBoard, MoveToken };

    struct Action {
        ActionType type{};
        int tokenIndex{};
        string description;
    };

    vector<Coord> path;
    array<Player, 2> players;
    int currentPlayer = 0;
    mt19937 rng;

public:
    LudoGame() : path(buildPath()), rng(random_device{}()) {
        players[0] = Player{"RED",  'R', 0,  vector<Token>(TOKEN_COUNT)};
        players[1] = Player{"BLUE", 'B', 12, vector<Token>(TOKEN_COUNT)};
    }

    void run() {
        printWelcome();

        while (true) {
            renderBoard();

            Player &player = players[currentPlayer];
            cout << "\n" << string(72, '=') << "\n";
            cout << "Turn: Player " << player.name << " (" << player.symbol << ")\n";
            waitForEnter("Press ENTER to roll the dice...");

            int dice = rollDice();
            cout << "\nDice rolled: " << dice << "\n";

            vector<Action> actions = getLegalActions(currentPlayer, dice);
            if (actions.empty()) {
                cout << "No legal move is available. Turn skipped.\n";
                switchTurn();
                continue;
            }

            int selectedAction = 0;
            if (actions.size() == 1) {
                cout << "Only legal move: " << actions[0].description << "\n";
            } else {
                cout << "\nChoose your move:\n";
                for (size_t i = 0; i < actions.size(); ++i) {
                    cout << "  " << (i + 1) << ". " << actions[i].description << "\n";
                }
                selectedAction = readInt("Enter option number: ", 1, static_cast<int>(actions.size())) - 1;
            }

            applyAction(currentPlayer, actions[selectedAction], dice);

            if (hasPlayerWon(currentPlayer)) {
                renderBoard();
                cout << "\n" << string(72, '=') << "\n";
                cout << "Player " << player.name << " wins the game!\n";
                cout << string(72, '=') << "\n";
                break;
            }

            if (dice == 6) {
                cout << "Player " << player.name << " rolled a 6, so they get another turn.\n";
            } else {
                switchTurn();
            }
        }
    }

private:
    static vector<Coord> buildPath() {
        vector<Coord> result;
        result.reserve(PATH_LENGTH);

        // Clockwise border path around a 7x7 board.
        for (int c = 0; c < BOARD_SIZE; ++c) result.push_back({0, c});
        for (int r = 1; r < BOARD_SIZE; ++r) result.push_back({r, BOARD_SIZE - 1});
        for (int c = BOARD_SIZE - 2; c >= 0; --c) result.push_back({BOARD_SIZE - 1, c});
        for (int r = BOARD_SIZE - 2; r > 0; --r) result.push_back({r, 0});

        return result;
    }

    void printWelcome() const {
        cout << string(72, '=') << "\n";
        cout << "                 RUBBLE LUDO - CONSOLE EDITION\n";
        cout << string(72, '=') << "\n";
        cout << "Rules:\n";
        cout << "  1. Each player has 4 tokens. Roll 6 to bring a token out of home.\n";
        cout << "  2. Move tokens clockwise around the board.\n";
        cout << "  3. You must roll the exact number needed to finish a token.\n";
        cout << "  4. Landing on an opponent on a normal cell sends that token home.\n";
        cout << "  5. Safe cells are marked S. Captures are not allowed on safe cells.\n";
        cout << "  6. Rolling 6 gives an extra turn.\n";
        cout << string(72, '=') << "\n";
    }

    int rollDice() {
        uniform_int_distribution<int> dice(1, 6);
        return dice(rng);
    }

    void switchTurn() {
        currentPlayer = 1 - currentPlayer;
    }

    static void waitForEnter(const string &message) {
        cout << message;
        string ignored;
        getline(cin, ignored);
    }

    static int readInt(const string &prompt, int minValue, int maxValue) {
        while (true) {
            cout << prompt;
            string input;
            getline(cin, input);

            stringstream ss(input);
            int value;
            char extra;

            if (ss >> value && !(ss >> extra) && value >= minValue && value <= maxValue) {
                return value;
            }

            cout << "Invalid input. Please enter a number from "
                 << minValue << " to " << maxValue << ".\n";
        }
    }

    static string centerText(const string &text, int width = 5) {
        if (static_cast<int>(text.size()) >= width) {
            return text.substr(0, width);
        }

        int leftPadding = (width - static_cast<int>(text.size())) / 2;
        int rightPadding = width - static_cast<int>(text.size()) - leftPadding;
        return string(leftPadding, ' ') + text + string(rightPadding, ' ');
    }

    static bool isInPlay(const Token &token) {
        return token.progress >= 0 && token.progress < FINISHED;
    }

    bool isSafeCell(int actualCell) const {
        return actualCell == 0 || actualCell == 6 || actualCell == 12 || actualCell == 18;
    }

    int actualCellOf(const Player &player, const Token &token) const {
        return (player.startOffset + token.progress) % PATH_LENGTH;
    }

    string tokenName(const Player &player, int tokenIndex) const {
        return string(1, player.symbol) + to_string(tokenIndex + 1);
    }

    int countFinishedTokens(int playerIndex) const {
        int finished = 0;
        for (const Token &token : players[playerIndex].tokens) {
            if (token.progress == FINISHED) {
                ++finished;
            }
        }
        return finished;
    }

    bool hasPlayerWon(int playerIndex) const {
        return countFinishedTokens(playerIndex) == TOKEN_COUNT;
    }

    int captureCountOnCell(int playerIndex, int actualCell) const {
        if (isSafeCell(actualCell)) {
            return 0;
        }

        int opponentIndex = 1 - playerIndex;
        int count = 0;

        for (const Token &token : players[opponentIndex].tokens) {
            if (isInPlay(token) && actualCellOf(players[opponentIndex], token) == actualCell) {
                ++count;
            }
        }

        return count;
    }

    vector<Action> getLegalActions(int playerIndex, int dice) const {
        vector<Action> actions;
        const Player &player = players[playerIndex];

        for (int i = 0; i < TOKEN_COUNT; ++i) {
            const Token &token = player.tokens[i];

            if (token.progress == HOME) {
                if (dice == 6) {
                    Action action;
                    action.type = ActionType::EnterBoard;
                    action.tokenIndex = i;
                    action.description = "Bring " + tokenName(player, i) + " out of home";
                    actions.push_back(action);
                }
                continue;
            }

            if (token.progress == FINISHED) {
                continue;
            }

            int targetProgress = token.progress + dice;
            if (targetProgress <= FINISHED) {
                Action action;
                action.type = ActionType::MoveToken;
                action.tokenIndex = i;

                string fromText = "step " + to_string(token.progress) + "/" + to_string(PATH_LENGTH);
                string toText;

                if (targetProgress == FINISHED) {
                    toText = "FINISH";
                } else {
                    Token temp;
                    temp.progress = targetProgress;
                    int targetCell = actualCellOf(player, temp);
                    toText = "step " + to_string(targetProgress) + "/" + to_string(PATH_LENGTH)
                           + " (cell " + to_string(targetCell + 1) + ")";

                    int captures = captureCountOnCell(playerIndex, targetCell);
                    if (captures > 0) {
                        toText += " and capture " + to_string(captures) + " opponent token";
                        if (captures > 1) toText += "s";
                    }
                }

                action.description = "Move " + tokenName(player, i) + " from " + fromText + " to " + toText;
                actions.push_back(action);
            }
        }

        return actions;
    }

    void applyAction(int playerIndex, const Action &action, int dice) {
        Player &player = players[playerIndex];
        Token &token = player.tokens[action.tokenIndex];

        cout << "\nSelected: " << action.description << "\n";

        if (action.type == ActionType::EnterBoard) {
            token.progress = 0;
            cout << tokenName(player, action.tokenIndex) << " entered the board.\n";
            return;
        }

        token.progress += dice;

        if (token.progress == FINISHED) {
            cout << tokenName(player, action.tokenIndex) << " reached the finish.\n";
            return;
        }

        int targetCell = actualCellOf(player, token);
        captureOpponentsOnCell(playerIndex, targetCell);
    }

    void captureOpponentsOnCell(int playerIndex, int actualCell) {
        if (isSafeCell(actualCell)) {
            cout << "Landed on a safe cell. No capture.\n";
            return;
        }

        int opponentIndex = 1 - playerIndex;
        Player &opponent = players[opponentIndex];
        int captured = 0;

        for (Token &opponentToken : opponent.tokens) {
            if (isInPlay(opponentToken) && actualCellOf(opponent, opponentToken) == actualCell) {
                opponentToken.progress = HOME;
                ++captured;
            }
        }

        if (captured > 0) {
            cout << "Capture! " << captured << " " << opponent.name << " token";
            if (captured > 1) cout << "s";
            cout << " sent back home.\n";
        } else {
            cout << "Moved safely.\n";
        }
    }

    string tokenPositionText(const Player &player, int tokenIndex) const {
        const Token &token = player.tokens[tokenIndex];

        if (token.progress == HOME) {
            return "HOME";
        }
        if (token.progress == FINISHED) {
            return "FINISH";
        }

        int actualCell = actualCellOf(player, token);
        return "step " + to_string(token.progress) + "/" + to_string(PATH_LENGTH)
             + ", cell " + to_string(actualCell + 1);
    }

    void printStatus() const {
        cout << "\nToken Status:\n";
        for (int p = 0; p < 2; ++p) {
            const Player &player = players[p];
            cout << "  " << setw(5) << left << player.name
                 << " | Finished: " << countFinishedTokens(p) << "/" << TOKEN_COUNT << " | ";

            for (int i = 0; i < TOKEN_COUNT; ++i) {
                cout << tokenName(player, i) << "=" << tokenPositionText(player, i);
                if (i != TOKEN_COUNT - 1) cout << "  |  ";
            }
            cout << "\n";
        }
    }

    void renderBoard() const {
        vector<vector<string>> cells(BOARD_SIZE, vector<string>(BOARD_SIZE, centerText(" ")));
        vector<vector<string>> occupants(PATH_LENGTH);

        for (int i = 0; i < PATH_LENGTH; ++i) {
            Coord c = path[i];
            cells[c.row][c.col] = isSafeCell(i) ? centerText("S") : centerText(".");
        }

        for (const Player &player : players) {
            for (int i = 0; i < TOKEN_COUNT; ++i) {
                const Token &token = player.tokens[i];
                if (isInPlay(token)) {
                    int cellIndex = actualCellOf(player, token);
                    occupants[cellIndex].push_back(tokenName(player, i));
                }
            }
        }

        for (int cellIndex = 0; cellIndex < PATH_LENGTH; ++cellIndex) {
            if (occupants[cellIndex].empty()) {
                continue;
            }

            Coord c = path[cellIndex];
            if (occupants[cellIndex].size() == 1) {
                cells[c.row][c.col] = centerText(occupants[cellIndex][0]);
            } else {
                bool hasRed = false;
                bool hasBlue = false;

                for (const string &name : occupants[cellIndex]) {
                    if (!name.empty() && name[0] == 'R') hasRed = true;
                    if (!name.empty() && name[0] == 'B') hasBlue = true;
                }

                if (hasRed && hasBlue) {
                    cells[c.row][c.col] = centerText("MIX");
                } else {
                    char symbol = hasRed ? 'R' : 'B';
                    cells[c.row][c.col] = centerText(string(1, symbol) + "+" + to_string(occupants[cellIndex].size()));
                }
            }
        }

        cout << "\n" << string(72, '-') << "\n";
        cout << "Board: S = safe cell, . = normal path, R+/B+ = stacked tokens\n\n";

        string border = "     +";
        for (int c = 0; c < BOARD_SIZE; ++c) {
            border += "-----+";
        }

        for (int r = 0; r < BOARD_SIZE; ++r) {
            cout << border << "\n";
            cout << "     |";
            for (int c = 0; c < BOARD_SIZE; ++c) {
                cout << cells[r][c] << "|";
            }
            cout << "\n";
        }
        cout << border << "\n";

        printStatus();
    }
};

int main() {
    LudoGame game;
    game.run();
    return 0;
}