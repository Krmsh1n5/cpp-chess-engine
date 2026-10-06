#ifndef JEU_H
#define JEU_H

#include <string>
#include <vector>
#include "pieces.h"

class Game {
private:
    int enPassantCol = -1;  // column of the pawn that just double-pushed
    int enPassantRow = -1;  // row of that pawn (-1 = no en passant available)

public:
    Game(); // constructor
    ~Game(); // destructor

    // Show the board and status.
    void display() const;

    // Single entry point for all input: handles "e2e4" moves, "O-O", "O-O-O".
    void tryMove(const std::string& input);

    Color getTurn() const { return turn; }

    // Returns true once checkmate, stalemate, or a draw rule has ended the game.
    bool isGameOver() const { return gameOver; }

    // Returns the board position in canonical form (a1..h8, comma-separated).
    std::string canonical_position() const;

    // Returns the game result string: "1-0", "0-1", "1/2-1/2", or "?-?".
    std::string getResult() const { return result; }

    // Allows main() to set the result for resign / draw / quit.
    void setResult(const std::string& r) { result = r; }

private:
    // Square indexes (0..7 for columns and rows).
    using Case = std::pair<int,int>; // (colonne, ligne) = (column, row)

    // 8x8 board which contains the Piece class
    // nullptr --> empty square.
    Piece* board[8][8];

    // Color of the player to move.
    Color turn;

    // Indicates whether the opponent king is in check.
    bool check;

    // Set to true once the game has ended (checkmate, stalemate, draw).
    // tryMove() and castle() are no-ops while this is true.
    bool gameOver = false;

    // Machine-readable result: "1-0", "0-1", "1/2-1/2", "?-?".
    std::string result = "?-?";

    // Human-readable reason the game ended, printed by display() after the board.
    std::string gameOverMessage;

    // Board snapshots for threefold repetition detection.
    // Each entry encodes piece positions + active color + en passant file.
    std::vector<std::string> positionHistory;

    // Half-move clock for the 50-move rule.
    // Reset on every pawn move or capture; incremented otherwise.
    int halfMoveClock = 0;
    
    // Convert a square string (e.g., "b1") to column and row indexes.
    Case parseSquare(const std::string& s) const;

    // Verify if the given column and row are valid board coordinates.
    bool isValidSquare(int col, int lig) const;

    // Verify if the given color's king is in check.
    bool isInCheck(Color roiCouleur) const;

    // Verify if a specific square is attacked by any opponent piece.
    bool isSquareAttacked(int col, int row, Color byColor) const;

    // Simulate a move on a copy and check for king in check.
    // epCol/epRow: current en passant target (-1 if none).
    bool putsInCheck(int fromCol, int fromRow, int toCol, int toRow,
                     int epCol, int epRow) const;

    // Returns true if the current player has at least one legal move.
    // Used to detect checkmate and stalemate.
    bool hasAnyLegalMove() const;

    // Returns true if neither side has enough material to deliver checkmate.
    // Covers: K vs K, K+N vs K, K+B vs K, K+B vs K+B (same-colour bishops).
    bool hasInsufficientMaterial() const;

    // Serialise the board position + active color + EP file into a string.
    // Used to track threefold repetition.
    std::string boardSnapshot() const;

    // Called at the end of every move and castle.
    // Detects and announces checkmate, stalemate, threefold repetition,
    // and the 50-move rule, and sets gameOver accordingly.
    void checkGameOver();

    // Execute a regular piece move (e.g. "e2" -> "e4").
    void move(const std::string& orig, const std::string& dest);

    // Execute castling. kingside=true for O-O, false for O-O-O.
    void castle(bool kingside);

    // Ask the player which piece to promote a pawn to and replace it on the board.
    // col/row: the square the pawn just reached.
    void promote(int col, int row);

    // Helper to get the symbol of a square for display.
    std::string squareSymbol(int col, int lig) const;

    // Helper to print a separator line for the board display.
    void separatorLine() const;

    // Delete all pieces and set the board to empty.
    void clearBoard();
};

#endif