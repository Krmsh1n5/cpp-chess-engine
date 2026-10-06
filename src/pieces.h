#ifndef PIECES_H
#define PIECES_H

#include <string>

enum Color { WHITE, BLACK }; // enum for color indication

class Piece {
public:
    Piece(Color c) // constructor
        : color(c) {}

    virtual ~Piece() {} // destructor
    // we use the virtual keyword for polymorphism
    // basically to be able to overwrite/override the function

    Color getColor() const { 
        return color; 
    }

    // col/row : indexes 0..7.
    // board : a 8x8 board of Piece*
    // Each piece moves differently and has its own isLegalMove function
    virtual bool isLegalMove(int fromCol, int fromRow, int toCol, int toRow, 
                             Piece* const board[8][8]) const = 0;
    /* const = 0; is used to indicate that this function has no
    implementation here, so the derived classes must implement it */

    // The unicode symbol of pieces
    virtual std::string symbol() const = 0;

    // Used for finding checks
    virtual bool isKing() const {
        return false; 
    }

    // Used for en passant detection in Game
    virtual bool isPawn() const {
        return false;
    }

protected:
    Color color;

    // Verify if the path between the two squares is free (excluding the edges)
    // Used for: Rook, Bishop, Queen
    bool isPathClear(int fromCol, int fromRow, int toCol, int toRow,
                     Piece* const board[8][8]) const {
        int dx = (toCol > fromCol) ? 1 : (toCol < fromCol) ? -1 : 0;
        int dy = (toRow > fromRow) ? 1 : (toRow < fromRow) ? -1 : 0;

        int x = fromCol + dx;
        int y = fromRow + dy;
        while (x != toCol || y != toRow) {
            if (board[x][y] != nullptr) return false;
            x += dx;
            y += dy;
        }
        return true;
    }
};


class King : public Piece {
public:
    King(Color c) : Piece(c), hasMoved(false) {}
    bool isLegalMove(int fromCol, int fromRow, int toCol, int toRow,
                           Piece* const board[8][8]) const override;
    std::string symbol() const override { return color == WHITE ? "♚" : "♔"; }
    bool isKing() const override { return true; }

    bool hasMoved; // set to true after the king's first move, blocks castling
};


class Queen : public Piece {
public:
    Queen(Color c) 
        : Piece(c) {}
        
    bool isLegalMove(int fromCol, int fromRow, int toCol, int toRow,
                     Piece* const board[8][8]) const override;

    std::string symbol() const override { return color == WHITE ? "♛" : "♕"; }
};


class Rook : public Piece {
public:
    Rook(Color c) : Piece(c), hasMoved(false) {}

    bool isLegalMove(int fromCol, int fromRow, int toCol, int toRow,
                     Piece* const board[8][8]) const override;

    std::string symbol() const override { return color == WHITE ? "♜" : "♖"; }

    bool hasMoved; // set to true after the rook's first move, blocks castling
};


class Bishop : public Piece {
public:
    Bishop(Color c) 
        : Piece(c) {}

    bool isLegalMove(int fromCol, int fromRow, int toCol, int toRow,
                     Piece* const board[8][8]) const override;

    std::string symbol() const override { return color == WHITE ? "♝" : "♗"; }
};


class Knight : public Piece {
public:
    Knight(Color c) : Piece(c) {}
    bool isLegalMove(int fromCol, int fromRow, int toCol, int toRow,
                     Piece* const board[8][8]) const override;

    std::string symbol() const override { return color == WHITE ? "♞" : "♘"; }
};


class Pawn : public Piece {
public:
    Pawn(Color c) 
        : Piece(c) {}

    bool isLegalMove(int fromCol, int fromRow, int toCol, int toRow,
                     Piece* const board[8][8]) const override;

    // En passant-aware overload: epCol/epRow are the coordinates of the
    // enemy pawn that just double-pushed (-1 if none available this turn).
    bool isLegalMove(int fromCol, int fromRow, int toCol, int toRow,
                     Piece* const board[8][8], int epCol, int epRow) const;

    std::string symbol() const override { return color == WHITE ? "♟" : "♙"; }
    bool isPawn() const override { return true; }
};

#endif