#include "jeu.h"
#include <iostream>
#include <cctype>
#include <stdexcept>

using namespace std;

// Implementation of the constructor
Game::Game() : turn(WHITE), check(false) {

    // Board creation
    for (int lig = 0; lig < 8; ++lig)
        for (int col = 0; col < 8; ++col)
            board[col][lig] = nullptr;

    // White pieces (row 0 = rank 1).
    board[0][0] = new Rook(WHITE);
    board[1][0] = new Knight(WHITE);
    board[2][0] = new Bishop(WHITE);
    board[3][0] = new Queen(WHITE);
    board[4][0] = new King(WHITE);
    board[5][0] = new Bishop(WHITE);
    board[6][0] = new Knight(WHITE);
    board[7][0] = new Rook(WHITE);

    // White pawns (row 1 = rank 2).
    for (int col = 0; col < 8; ++col)
        board[col][1] = new Pawn(WHITE);

    // Black pieces (row 7 = rank 8).
    board[0][7] = new Rook(BLACK);
    board[1][7] = new Knight(BLACK);
    board[2][7] = new Bishop(BLACK);
    board[3][7] = new Queen(BLACK);
    board[4][7] = new King(BLACK);
    board[5][7] = new Bishop(BLACK);
    board[6][7] = new Knight(BLACK);
    board[7][7] = new Rook(BLACK);

    // Black pawns (row 6 = rank 7).
    for (int col = 0; col < 8; ++col)
        board[col][6] = new Pawn(BLACK);
}


Game::~Game() {
    clearBoard(); // Upon calling the destructor the board is auto-cleared
}


Game::Case Game::parseSquare(const string& s) const {
    if (s.size() != 2) throw invalid_argument("Invalid coordinate.");
    char colChar = tolower(s[0]); // tolower as columns are indicated as letters a-h
    char rowChar = s[1];
    
    // binary char checker
    if (colChar < 'a' || colChar > 'h' || rowChar < '1' || rowChar > '8')
        throw invalid_argument("Coordinate off the board.");
    
    // char binary arithmetic turned to int
    int col = colChar - 'a';
    int lig = rowChar - '1';
    return {col, lig};
}

// Check if the given coordinates are in the board boundary
bool Game::isValidSquare(int col, int lig) const {
    return col >= 0 && col < 8 && lig >= 0 && lig < 8;
}


bool Game::isInCheck(Color roiCouleur) const {
    int kingCol = -1, kingRow = -1; // not possible king position

    // Firstly we get the king's position.
    /* We go through the board and try to find 
    our wanted King piece on each square */
    for (int lig = 0; lig < 8; ++lig) {
        for (int col = 0; col < 8; ++col) {
            Piece* p = board[col][lig];
            /* If the square is not empty and the color of the
            piece is correct and the piece is the King piece 
            Then we write down the King's actual position */
            if (p && p->getColor() == roiCouleur && p->isKing()) {
                kingCol = col;
                kingRow = lig;
                break;
            }
        }
        if (kingCol != -1) break;
    }

    if (kingCol == -1) return false; // Not supposed to happen in a valid game.

    // the opposing color of our King piece
    Color adversaire = (roiCouleur == WHITE) ? BLACK : WHITE; 
    // We check if any opponent piece can move to the king's square
    // If it can --> return true  |  Otherwise --> return false
    for (int lig = 0; lig < 8; ++lig) {
        for (int col = 0; col < 8; ++col) {
            Piece* p = board[col][lig];
            
            if (p && p->getColor() == adversaire) {
                if (p->isLegalMove(col, lig, kingCol, kingRow, board))
                    return true;
            }
        }
    }
    return false;
}

// Checks if a given square is attacked by any piece of byColor.
// Reuses each piece's isLegalMove — same logic as isInCheck but for any square.
bool Game::isSquareAttacked(int col, int row, Color byColor) const {
    for (int lig = 0; lig < 8; ++lig) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = board[c][lig];
            if (p && p->getColor() == byColor) {
                if (p->isLegalMove(c, lig, col, row, board))
                    return true;
            }
        }
    }
    return false;
}

bool Game::putsInCheck(int fromCol, int fromRow, int toCol, int toRow,
                       int epCol, int epRow) const {
    // Create a copy of the board to simulate the move.
    Piece* copie[8][8];
    for (int lig = 0; lig < 8; ++lig)
        for (int col = 0; col < 8; ++col)
            copie[col][lig] = board[col][lig]; // shallow copy

    // Simulate the move on the copy
    copie[toCol][toRow] = copie[fromCol][fromRow];
    copie[fromCol][fromRow] = nullptr;

    // If this was an en passant capture, also remove the captured pawn
    // from the simulation (it sits beside the capturing pawn, not on toRow).
    Piece* moved = copie[toCol][toRow];
    if (moved && moved->isPawn() && epCol != -1 &&
        toCol == epCol && toRow == epRow + ((moved->getColor() == WHITE) ? 1 : -1)) {
        copie[epCol][epRow] = nullptr; // remove the sideways pawn
    }

    // Check if the player's king is in check after this move.
    int kingCol = -1, kingRow = -1;
    for (int lig = 0; lig < 8; ++lig) {
        for (int col = 0; col < 8; ++col) {
            Piece* p = copie[col][lig];
            if (p && p->getColor() == turn && p->isKing()) {
                kingCol = col;
                kingRow = lig;
                break;
            }
        }
    }
    if (kingCol == -1) return false;

    Color adversaire = (turn == WHITE) ? BLACK : WHITE;
    for (int lig = 0; lig < 8; ++lig) {
        for (int col = 0; col < 8; ++col) {
            Piece* p = copie[col][lig];
            if (p && p->getColor() == adversaire) {
                if (p->isLegalMove(col, lig, kingCol, kingRow, copie))
                    return true;
            }
        }
    }
    return false;
}


string Game::squareSymbol(int col, int lig) const {
    Piece* p = board[col][lig];
    if (p) {
        return string("  ") + p->symbol() + "  ";
    } else {
        return "     "; 
    }
}


void Game::separatorLine() const {
    cout << " +-----+-----+-----+-----+-----+-----+-----+-----+" << endl;
}

void Game::clearBoard() {
    for (int lig = 0; lig < 8; ++lig)
        for (int col = 0; col < 8; ++col)
            delete board[col][lig];
}


void Game::display() const {
    cout << "    a     b     c     d     e     f     g     h    " << endl;

    for (int lig = 7; lig >= 0; --lig) { // ranks 8..1
        separatorLine();
        cout << " |";
        for (int col = 0; col < 8; ++col) {
            cout << squareSymbol(col, lig) << "|";
        }
        cout << " " << (lig+1);
        cout << endl;
    }

    separatorLine();

    // If the game has ended, show the result. Otherwise show whose turn it is.
    if (gameOver)
        cout << gameOverMessage << endl;
    else
        cout << "[" << (turn == WHITE ? "White" : "Black") << "]"
             << (check ? " Check!" : "") << endl;
}


void Game::move(const string& orig, const string& dest) {
    try {
        if (gameOver) {
            cerr << "The game is already over." << endl;
            return;
        }

        auto [fromCol, fromRow] = parseSquare(orig);
        auto [toCol, toRow] = parseSquare(dest);

        if (!isValidSquare(fromCol, fromRow) || !isValidSquare(toCol, toRow)) {
            throw runtime_error("Coordonnées hors échiquier.");
        }

        Piece* piece = board[fromCol][fromRow];
        if (!piece) {
            throw runtime_error("Empty square.");
        }
        if (piece->getColor() != turn) {
            throw runtime_error("That's not your piece.");
        }

        /* Snapshot the current En Passant state before resetting it.
        En Passant is only valid for exactly one turn, so we capture it here
        to validate this move, then it will be overwritten below. */
        int currentEpCol = enPassantCol;
        int currentEpRow = enPassantRow;

        /* Check move legality: pawns have a separate isLegalMove function,
        which is overloaded because of En Passant checking. */
        bool legal = false;
        if (piece->isPawn()) {
            Pawn* pawn = static_cast<Pawn*>(piece);
            legal = pawn->isLegalMove(fromCol, fromRow, toCol, toRow, board,
                                      currentEpCol, currentEpRow);
        } else {
            legal = piece->isLegalMove(fromCol, fromRow, toCol, toRow, board);
        }

        if (!legal) {
            throw runtime_error("Illegal move for that piece.");
        }

        // Detect en passant capture *before* moving anything on the board:
        /* 1. Piece is a pawn
           2. The move is diagonal, given that toCol != fromCol
           3. The square to which it moves to is empty
           4. There is a valid En Passant target, given that 
           currentEpCol != -1 and toCol == currentEpCol*/
        bool isEnPassantCapture = false;
        if (piece->isPawn() && currentEpCol != -1 && toCol == currentEpCol && 
            board[toCol][toRow] == nullptr && toCol != fromCol) { // diagonal move
            isEnPassantCapture = true;
        }

        // Can't put yourself in check (pass En Passant state so the simulation is accurate).
        if (putsInCheck(fromCol, fromRow, toCol, toRow, currentEpCol, currentEpRow)) {
            throw runtime_error("This move would leave your king in check.");
        }

        // --- All checks passed: execute the move ---

        // Reset en passant: by default no EP is available next turn.
        enPassantCol = -1;
        enPassantRow = -1;

        // Update the half-move clock BEFORE touching the board:
        // reset on pawn move or capture, increment otherwise.
        bool isCapture = (board[toCol][toRow] != nullptr) || isEnPassantCapture;
        if (piece->isPawn() || isCapture)
            halfMoveClock = 0;
        else
            ++halfMoveClock;

        // If this is an en passant capture, delete the captured pawn sideways.
        if (isEnPassantCapture) {
            delete board[currentEpCol][currentEpRow];
            board[currentEpCol][currentEpRow] = nullptr;
        }

        // Standard capture at destination square.
        if (board[toCol][toRow]) {
            delete board[toCol][toRow];
            board[toCol][toRow] = nullptr;
        }

        board[toCol][toRow] = piece;
        board[fromCol][fromRow] = nullptr;

        // Mark king/rook as having moved so castling rights are lost.
        if (piece->isKing())
            static_cast<King*>(piece)->hasMoved = true;
        else if (dynamic_cast<Rook*>(piece))
            static_cast<Rook*>(piece)->hasMoved = true;

        // Remember whether the moving piece was a pawn.
        // Important: after promotion the original pawn object is deleted,
        // so we should not use `piece->isPawn()` afterwards.
        bool wasPawn = piece->isPawn();

        // Determine pawn movement direction:
        // white pawns move upward (+1 row), black downward (-1 row).
        int direction = (turn == WHITE) ? 1 : -1;

        // Detect a double pawn push (needed for en passant on the next turn).
        bool isDoublePawnPush = wasPawn && (toRow - fromRow == 2 * direction);

        // Store the en passant target square if a pawn moved two squares.
        if (isDoublePawnPush) {
            enPassantCol = toCol;
            enPassantRow = toRow;
        }

        // Pawn promotion: white promotes on row 7, black on row 0.
        int promotionRow = (turn == WHITE) ? 7 : 0;

        // Replace the pawn with the chosen promoted piece.
        if (wasPawn && toRow == promotionRow) {
            promote(toCol, toRow);
        }

        check = false;
        turn = (turn == WHITE) ? BLACK : WHITE;

        if (isInCheck(turn))
            check = true;

        checkGameOver();

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return;
    }
}

bool Game::hasAnyLegalMove() const {

    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            Piece* p = board[fromCol][fromRow];

            // If empty square or piece is of wrong color
            if (!p || p->getColor() != turn) continue;

            // Try every possible destination square.
            for (int toRow = 0; toRow < 8; ++toRow) {
                for (int toCol = 0; toCol < 8; ++toCol) {
                    // do not consider the same square
                    if (fromCol == toCol && fromRow == toRow) continue;

                    bool legal = false;
                    // Check if the moves are legal given the type of the piece
                    if (p->isPawn()) {
                        // Must use EP-aware overload so En Passant captures are considered.
                        legal = static_cast<Pawn*>(p)->isLegalMove(
                            fromCol, fromRow, toCol, toRow,
                            board, enPassantCol, enPassantRow);
                    } else {
                        legal = p->isLegalMove(fromCol, fromRow, toCol, toRow, board);
                    }

                    // If the move is legal and doesn't put the king in check
                    if (legal && !putsInCheck(fromCol, fromRow, toCol, toRow,
                                              enPassantCol, enPassantRow))
                        return true;
                }
            }
        }
    }

    // We also check whether castling is available as a legal move.
    // We attempt both sides; if the castle() logic would succeed the king has a legal move.
    int row = (turn == WHITE) ? 0 : 7;
    Color opponent = (turn == WHITE) ? BLACK : WHITE;

    Piece* kingPiece = board[4][row];
    /* 1. If the square is not empty and
       2. The piece is a King piece and
       3. The piece king piece hasn't moved and
       4. The King is not in check*/
    if (kingPiece && kingPiece->isKing() && !static_cast<King*>(kingPiece)->hasMoved
        && !isInCheck(turn)) {
        // We try both the kingside and queenside castling in order
        for (bool kingside : {true, false}) {
            // Depending on the castling side, fetch the rook position
            int rookCol = kingside ? 7 : 0;
            Piece* rookPiece = board[rookCol][row];
            
            // If the square is empty or the piece is not a rook --> continue;
            if (!rookPiece || !dynamic_cast<Rook*>(rookPiece)) continue;
            // If the rook has moved --> continue;
            if (static_cast<Rook*>(rookPiece)->hasMoved) continue;

            // Check squares between king and rook are empty.
            int step = kingside ? 1 : -1;
            bool pathClear = true;
            for (int c = 4 + step; c != rookCol; c += step)
                if (board[c][row]) { pathClear = false; break; }
            if (!pathClear) continue;

            // Check king doesn't pass through or land on an attacked square.
            int kingDest = kingside ? 6 : 2;
            bool attacked = false;
            for (int c = 4 + step; ; c += step) {
                if (isSquareAttacked(c, row, opponent)) { attacked = true; break; }
                if (c == kingDest) break;
            }
            if (!attacked) return true;
        }
    }

    // If there no more legal moves for any of the pieces --> return False
    return false;
}


std::string Game::boardSnapshot() const {
    // Encode each square as a two-character token: color + piece letter.
    // Empty squares are "..", active color is appended, then EP file (or '-').
    std::string s;
    s.reserve(64 * 2 + 2);
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            // Fetch the piece
            Piece* p = board[col][row];
            // If square is empty store as ".." (2 dots)
            if (!p) { s += ".."; continue; }
            
            s += (p->getColor() == WHITE) ? 'W' : 'B';
            // One-character piece code.
            if      (p->isKing())               s += 'K';
            else if (p->isPawn())               s += 'P';
            else if (dynamic_cast<Queen*>(p))   s += 'Q';
            else if (dynamic_cast<Rook*>(p))    s += 'R';
            else if (dynamic_cast<Bishop*>(p))  s += 'B';
            else                                s += 'N'; // Knight
        }
    }
    s += (turn == WHITE) ? 'W' : 'B';
    s += (enPassantCol == -1) ? '-' : static_cast<char>('a' + enPassantCol);
    return s;
}


bool Game::hasInsufficientMaterial() const {
    /* Struct to collect the info of all the pieces that are on the board, 
    The info being, their color, type, and the squre color for bishop pieces */
    // Square color: (col + row) % 2.
    struct PieceInfo { Color color; char type; int squareColor; };
    std::vector<PieceInfo> pieces;

    // Go through the board and collect each piece that is on it
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {

            Piece* p = board[col][row];
            if (!p) continue; // skip if the square is empty --> no Piece

            char type = '?';
            if      (p->isKing())              type = 'K';
            else if (p->isPawn())              type = 'P';
            else if (dynamic_cast<Queen*>(p))  type = 'Q';
            else if (dynamic_cast<Rook*>(p))   type = 'R';
            else if (dynamic_cast<Bishop*>(p)) type = 'B';
            else                               type = 'N'; // Knight

            // Any pawn, queen, or rook means sufficient material.
            // Which means that checkmate is still possible so --> return false;
            if (type == 'P' || type == 'Q' || type == 'R') return false;

            pieces.push_back({p->getColor(), type, (col + row) % 2});
        }
    }

    // Count minor pieces (bishop or knight only) per side.
    int whiteMinors = 0, blackMinors = 0;
    int whiteBishopSquare = -1, blackBishopSquare = -1;

    for (const auto& pi : pieces) {
        if (pi.type == 'K') continue;
        if (pi.color == WHITE) {
            ++whiteMinors;
            if (pi.type == 'B') whiteBishopSquare = pi.squareColor;
        } else {
            ++blackMinors;
            if (pi.type == 'B') blackBishopSquare = pi.squareColor;
        }
    }

    // K vs K
    if (whiteMinors == 0 && blackMinors == 0) return true;

    // K+minor vs K (single bishop or knight on either side, nothing on the other)
    if (whiteMinors == 1 && blackMinors == 0) return true;
    if (whiteMinors == 0 && blackMinors == 1) return true;

    // K+B vs K+B with bishops on the same square color: neither can ever capture
    if (whiteMinors == 1 && blackMinors == 1 &&
        whiteBishopSquare != -1 && blackBishopSquare != -1 &&
        whiteBishopSquare == blackBishopSquare) return true;

    return false;
}

void Game::checkGameOver() {
    // --- Insufficient material ---
    if (hasInsufficientMaterial()) {
        gameOverMessage = "Draw by insufficient material.";
        result = "1/2-1/2";
        gameOver = true;
        return;
    }

    // --- Threefold repetition ---
    std::string snap = boardSnapshot();
    positionHistory.push_back(snap);
    int count = 0;
    for (const auto& h : positionHistory)
        if (h == snap) ++count;
    if (count >= 3) {
        gameOverMessage = "Draw by threefold repetition.";
        result = "1/2-1/2";
        gameOver = true;
        return;
    }

    // --- 50-move rule ---
    if (halfMoveClock >= 100) {
        gameOverMessage = "Draw by the 50-move rule.";
        result = "1/2-1/2";
        gameOver = true;
        return;
    }

    // --- Checkmate / stalemate ---
    // Checked last: most expensive (scans all pieces × all squares).
    if (!hasAnyLegalMove()) {
        // If no legal moves, and the king is in Check --> checkmate
        if (isInCheck(turn)) {
            Color winner = (turn == WHITE) ? BLACK : WHITE;
            gameOverMessage = std::string("Checkmate! ")
                            + (winner == WHITE ? "White" : "Black") + " wins.";
            result = (winner == WHITE) ? "1-0" : "0-1";
        } 
        // Otherwise --> stalemate
        else {
            gameOverMessage = "Stalemate! The game is a draw.";
            result = "1/2-1/2";
        }
        gameOver = true;
    }
}

void Game::promote(int col, int row) {
    Color c = board[col][row]->getColor();
    char choice = 0;

    // Keep asking until the player gives a valid character.
    while (true) {
        cout << "Promote pawn to? [Q]ueen  [R]ook  [B]ishop  [N]knight : ";
        cin >> choice;
        choice = toupper(choice);
        if (choice == 'Q' || choice == 'R' || choice == 'B' || choice == 'N')
            break;
        cerr << "Invalid choice. Please enter Q, R, B, or N." << endl;
    }

    // Delete the pawn that just reached the back rank.
    delete board[col][row];
    board[col][row] = nullptr;

    // Place the chosen piece on the same square.
    switch (choice) {
        case 'Q': board[col][row] = new Queen(c);  break;
        case 'R': board[col][row] = new Rook(c);   break;
        case 'B': board[col][row] = new Bishop(c); break;
        case 'N': board[col][row] = new Knight(c); break;
    }
}


void Game::castle(bool kingside) {
    try {
        // if king white --> first row  |  Otherwise --> last row
        int row = (turn == WHITE) ? 0 : 7;
        Color opponent = (turn == WHITE) ? BLACK : WHITE; // opponent color

        // --- 1. Locate the king and verify it hasn't moved ---
        int kingCol = 4; // Both Kings always start on the e-file (col 4)
        Piece* kingPiece = board[kingCol][row];
        
        /* If the starting position is empty or
        The piece is not the King piece or
        The color of the king doesn't match
        Then --> cannot castle */
        if (!kingPiece || !kingPiece->isKing() || kingPiece->getColor() != turn)
            throw runtime_error("King not on its starting square.");

        // If the King piece has moved, also cannot castle
        if (static_cast<King*>(kingPiece)->hasMoved)
            throw runtime_error("Cannot castle: king has already moved.");

        // --- 2. Locate the rook and verify it hasn't moved ---
        // Determine the rook column, depending if short or long castling
        int rookCol = kingside ? 7 : 0; 
        Piece* rookPiece = board[rookCol][row];

        /* If the starting pos is empty or
        the piece is not a rook piece or
        the rook color is incorrect 
        Then --> cannot castle*/
        if (!rookPiece || !dynamic_cast<Rook*>(rookPiece) || rookPiece->getColor() != turn)
            throw runtime_error("Rook not on its starting square.");
        
        // If the rook hasn moved --> cannot castle
        if (static_cast<Rook*>(rookPiece)->hasMoved)
            throw runtime_error("Cannot castle: rook has already moved.");

        // --- 3. All squares between king and rook must be empty ---
        // Kingside: cols 5,6 must be empty.  Queenside: cols 1,2,3 must be empty.
        /* If long short castle --> the step (1) is positive, so goes towards columns 5, 6
        Otherwise --> the step is negative, checking the 1,2,3 columns */
        int step = kingside ? 1 : -1; 
        for (int c = kingCol + step; c != rookCol; c += step) {
            if (board[c][row] != nullptr) // if there is a piece inbetween --> not possible to castle
                throw runtime_error("Cannot castle: pieces between king and rook.");
        }

        // --- 4. King must not currently be in check ---
        if (isInCheck(turn))
            throw runtime_error("Cannot castle while in check.");

        // --- 5. King must not pass through or land on an attacked square ---
        // Kingside: king travels e→f→g (cols 4→5→6).
        // Queenside: king travels e→d→c (cols 4→3→2).
        int kingDest = kingside ? 6 : 2;
        for (int c = kingCol + step; ; c += step) { // step has already been chosen given the type of castling
            if (isSquareAttacked(c, row, opponent))
                throw runtime_error("Cannot castle: king passes through or lands on attacked square.");
            if (c == kingDest) break;
        }

        // --- All checks passed: execute castling ---

        // Move king to its destination square.
        board[kingDest][row] = kingPiece;
        board[kingCol][row]  = nullptr;
        static_cast<King*>(kingPiece)->hasMoved = true;

        // Move rook to the square the king just crossed.
        int rookDest = kingside ? 5 : 3;
        board[rookDest][row] = rookPiece;
        board[rookCol][row]  = nullptr;
        static_cast<Rook*>(rookPiece)->hasMoved = true;

        // En passant is always reset after any move.
        enPassantCol = -1;
        enPassantRow = -1;

        check = false;
        turn = (turn == WHITE) ? BLACK : WHITE;
        if (isInCheck(turn))
            check = true;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

std::string Game::canonical_position() const {
    std::string output;
    // Traverse a1→h1, then a2→h2, …, a8→h8.
    // board[col][row]: col 0='a', row 0=rank1.
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* p = board[col][row];
            if (p) {
                output += (p->getColor() == WHITE) ? 'w' : 'b';
                if      (p->isKing())                 output += 'K';
                else if (p->isPawn())                 output += 'P';
                else if (dynamic_cast<Queen*>(p))     output += 'Q';
                else if (dynamic_cast<Rook*>(p))      output += 'R';
                else if (dynamic_cast<Bishop*>(p))    output += 'B';
                else                                  output += 'N'; // Knight
            }
            output += ',';
        }
    }
    return output;
}

// Responsible for castling and regular moves
void Game::tryMove(const string& input) {
    /* Castling input: "O-O" (kingside) or "O-O-O" (queenside).
    We check the string contents, to determine
    to castle or make a reqular move */
    if (input == "O-O") {
        castle(true);
        return;
    }
    if (input == "O-O-O") {
        castle(false);
        return;
    }

    // Regular move: must be exactly 4 characters (e.g. "e2e4").
    if (input.size() != 4) {
        cerr << "Error: Unknown command. Use e.g. 'e2e4', 'O-O', or 'O-O-O'." << endl;
        return;
    }

    string orig = input.substr(0, 2);
    string dest = input.substr(2, 2);
    move(orig, dest);
}