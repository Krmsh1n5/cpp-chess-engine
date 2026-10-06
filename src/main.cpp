#include "jeu.h"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <unistd.h>

using namespace std;

// Validates a standard move in algebraic notation (e.g. "e2e4").
bool is_valid_move(string const& cmd) {
    regex mouvmtpattern("[a-h][1-8][a-h][1-8]");
    return regex_match(cmd, mouvmtpattern);
}

// Validates a kingside castling command (O-O, o-o, 0-0, etc.).
bool is_valid_move_kingside_castle(string const& cmd) {
    regex mouvmtpattern("(O|o|0)-(O|o|0)");
    return regex_match(cmd, mouvmtpattern);
}

// Validates a queenside castling command (O-O-O, o-o-o, 0-0-0, etc.).
bool is_valid_move_queenside_castle(string const& cmd) {
    regex mouvmtpattern("(O|o|0)-(O|o|0)-(O|o|0)");
    return regex_match(cmd, mouvmtpattern);
}

int main(int argc, char* argv[]) {
    Game mygame;
    string move;
    bool stop(false);

    mygame.display();

    // Input source
    istream* input = &cin;

    // If a filename was provided
    if (argc > 1) {
        ifstream* file = new ifstream(argv[1]);

        if (!file->is_open()) {
            cerr << "Failed to open file: " << argv[1] << endl;
            return 1;
        }

        input = file;
    }

    do {
        // Only show prompt for terminal input
        if (isatty(STDIN_FILENO))
            cout << "Move (eg. e2e4) ? ";

        if (!getline(*input, move))
            break;

        // Strip trailing whitespace (spaces, carriage returns from Windows-format files).
        while (!move.empty() && (move.back() == '\r' || move.back() == ' ' || move.back() == '\t'))
            move.pop_back();

        if (move == "/quit") {
            // result stays "?-?" (interrupted game)
            stop = true;

        } else if (move == "/resign") {
            // The player whose turn it is resigns; the opponent wins.
            if (mygame.getTurn() == WHITE) {
                mygame.setResult("0-1");
                cout << "Resignation by White. Black wins." << endl;
            } else {
                mygame.setResult("1-0");
                cout << "Resignation by Black. White wins." << endl;
            }
            stop = true;

        } else if (move == "/draw") {
            mygame.setResult("1/2-1/2");
            cout << "Draw." << endl;
            stop = true;

        } else if (is_valid_move(move)) {
            // Standard move validated by regex (e.g. "e2e4").
            mygame.tryMove(move);
            mygame.display();
            if (mygame.isGameOver())
                stop = true;

        } else if (is_valid_move_queenside_castle(move)) {
            // Queenside castling: O-O-O (also accepts o-o-o, 0-0-0, etc.).
            mygame.tryMove("O-O-O");
            mygame.display();
            if (mygame.isGameOver())
                stop = true;

        } else if (is_valid_move_kingside_castle(move)) {
            // Kingside castling: O-O (also accepts o-o, 0-0, etc.).
            // Note: queenside must be checked first (O-O-O also matches O-O).
            mygame.tryMove("O-O");
            mygame.display();
            if (mygame.isGameOver())
                stop = true;

        } else {
            cerr << "Error: invalid input \"" << move << "\". "
                 << "Use e.g. 'e2e4', 'O-O', 'O-O-O', '/resign', '/draw', '/quit'." << endl;
        }

    } while (!stop);

    // Final output line required by the test framework:
    // canonical board position followed by the game result.
    cout << mygame.canonical_position() << " " << mygame.getResult() << endl;

    return 0;
}