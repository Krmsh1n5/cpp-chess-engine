#include "pieces.h"
#include <cstdlib>

bool King::isLegalMove(int fromCol, int fromRow, int toCol, int toRow,
                       Piece* const board[8][8]) const {

    int dCol = abs(toCol - fromCol); // col diff
    int dRow = abs(toRow - fromRow); // row diff

    if (dCol <= 1 && dRow <= 1 && (dCol + dRow != 0)) {
        // Verify that the destination square is not already occupied by a friendly piece
        Piece* dest = board[toCol][toRow];

        /* if our destination coord doesn't have a piece
        Then: dest = nullptr --> !dest = true
        Otherwise check for the color of the piece */
        return !dest || dest->getColor() != color;
    }

    return false;
}

bool Queen::isLegalMove(int fromCol, int fromRow, int toCol, int toRow,
                        Piece* const board[8][8]) const {

    int dCol = abs(toCol - fromCol);
    int dRow = abs(toRow - fromRow);

    if (!(fromCol == toCol || fromRow == toRow || dCol == dRow))
        return false;

    if (!isPathClear(fromCol, fromRow, toCol, toRow, board))
        return false;

    Piece* dest = board[toCol][toRow];

    /* isPathClear doesn't check the destination position so we apply 
    the same logic of checking the dest pos as we did with the King 
    This logic is applied for all the pieces */
    return !dest || dest->getColor() != color;
}

bool Rook::isLegalMove(int fromCol, int fromRow, int toCol, int toRow,
                       Piece* const board[8][8]) const {
    if (fromCol != toCol && fromRow != toRow)
        return false;
    if (!isPathClear(fromCol, fromRow, toCol, toRow, board))
        return false;
    Piece* dest = board[toCol][toRow];
    return !dest || dest->getColor() != color;
}

bool Bishop::isLegalMove(int fromCol, int fromRow, int toCol, int toRow,
                         Piece* const board[8][8]) const {

    int dCol = abs(toCol - fromCol);
    int dRow = abs(toRow - fromRow);

    if (dCol != dRow)
        return false;

    if (!isPathClear(fromCol, fromRow, toCol, toRow, board))
        return false;

    Piece* dest = board[toCol][toRow];
    return !dest || dest->getColor() != color;
}

bool Knight::isLegalMove(int fromCol, int fromRow, int toCol, int toRow,
                         Piece* const board[8][8]) const {
    int dCol = abs(toCol - fromCol);
    int dRow = abs(toRow - fromRow);

    if (!((dCol == 2 && dRow == 1) || (dCol == 1 && dRow == 2)))
        return false;

    Piece* dest = board[toCol][toRow];
    return !dest || dest->getColor() != color;
}

bool Pawn::isLegalMove(int fromCol, int fromRow, int toCol, int toRow,
                       Piece* const board[8][8]) const {

    // depending on the color the straight direction of the pawn is different
    int direction = (color == WHITE) ? 1 : -1;
    int startingRank = (color == WHITE) ? 1 : 6; // rank 2 (index 1) or 7 (index 6)
    int dCol = toCol - fromCol;
    int dRow = toRow - fromRow;

    Piece* dest = board[toCol][toRow];

    /* If the pawn doesn't go out of its column
    and the row difference matches the color piece direction 
    Then: check if the dest pos has no piece
    If no piece on dest pos --> dest = nullptr 
    This way the expression dest == nullptr will be true*/
    if (dCol == 0 && dRow == direction) {
        return dest == nullptr;
    }

    /* If the pawn is on its starting position and
    we want to go 2 up, depending on the color of the piece*/
    if (dCol == 0 && dRow == 2 * direction && fromRow == startingRank) {
        // The intermediate square must be empty
        int interLig = fromRow + direction;
        if (board[fromCol][interLig] != nullptr) return false;
        return dest == nullptr;
    }

    // Diagonal capture
    if (abs(dCol) == 1 && dRow == direction) {
        /* check that there is a piece and 
        the piece is of different color */
        return dest && dest->getColor() != color;
    }

    return false;
}

bool Pawn::isLegalMove(int fromCol, int fromRow, int toCol, int toRow,
                       Piece* const board[8][8], int epCol, int epRow) const {

    // All normal pawn rules still apply first
    if (isLegalMove(fromCol, fromRow, toCol, toRow, board))
        return true;

    // En passant is only triggered when valid EP state exists
    if (epCol == -1) return false;

    int direction = (color == WHITE) ? 1 : -1;
    int dCol = toCol - fromCol;
    int dRow = toRow - fromRow;

    /* The capturing pawn moves one square diagonally forward.
    The destination square is empty (no piece there to capture normally).
    The target square is the one directly behind the enemy pawn that
    just double-pushed: that pawn sits at (epCol, epRow), and we land
    one step in our forward direction from it. */
    if (abs(dCol) == 1 && dRow == direction) {
        Piece* dest = board[toCol][toRow];
        if (!dest && toCol == epCol && toRow == epRow + direction) {
            return true;
        }
    }

    return false;
}