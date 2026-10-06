# cpp-chess-engine

A full-rules chess engine written in modern C++ (C++17), played from the terminal. It renders the board with Unicode pieces, validates every move against the real rules of chess, and recognises check, checkmate, stalemate, and the standard draw conditions. There is no AI opponent — the program is a complete, rule-enforcing referee for two human players (or for a scripted game fed from a file).

The design is built around a polymorphic `Piece` hierarchy with virtual dispatch, so the board and game logic never branch on piece type. That OOP core is the point of the project; the chess rules are the exercise that drives it.

**Authors:** Dmitriy Kuramshin, Kamal Yalchin, Toghrul Mardiyev, Luis Markus Torres, Farid Veliyev

## Build and run

Requires a C++17 compiler (`g++` or `clang++`) and `make`.

```bash
make -C src          # builds src/chess
```

Play interactively:

```bash
./src/chess
```

Or replay a game from a move file (one move per line, same format as interactive input):

```bash
./src/chess data/4-leg-mat-berger.txt
```

Clean build artifacts with `make -C src clean`.

### How to play

Moves use coordinate notation — origin square followed by destination square, e.g. `e2e4`. Castling is entered as `O-O` (kingside) or `O-O-O` (queenside); the letter `O`, lowercase `o`, and digit `0` are all accepted. The following commands work at any prompt:

| Command | Effect |
|---------|--------|
| `e2e4`  | Move a piece (algebraic coordinate notation) |
| `O-O` / `O-O-O` | Kingside / queenside castling |
| `/resign` | Resign; the opponent wins |
| `/draw`   | Agree to a draw |
| `/quit`   | Abandon the game (no result recorded) |

When a pawn reaches the back rank, the program prompts for the promotion piece (Queen, Rook, Bishop, or Knight).

## Testing

The `test/` harness replays recorded games from `data/` and compares the engine's final board position and result against a reference line stored at the end of each file. Test games are grouped into four levels matching the feature set, and named `leg` (legal game) or `ill` (contains an illegal move that must be rejected).

Run a level from the repository root:

```bash
./test/test-level.sh 1        # legal + illegal games for level 1
./test/test-level.sh 4        # mates and stalemates
```

All 42 bundled games pass across levels 1–4.

## Features

The engine implements four cumulative levels of functionality:

1. **Movement and board rules** — per-piece move geometry, path-blocking for sliding pieces, captures, the pawn's two-square opening advance, and strict turn alternation.
2. **Check detection** — after each move the active king is tested for check; any move that would leave or place one's own king in check is rejected before it is applied.
3. **Special moves** — kingside and queenside castling (with full legality checks on the king's path), en passant (valid only on the half-move after a double pawn push), and pawn promotion.
4. **Game termination** — checkmate and stalemate detection, plus three draw rules: the fifty-move rule, threefold repetition, and insufficient mating material.

## Architecture and OOP design

The codebase is deliberately small (~1,250 lines) and splits cleanly into two responsibilities: **chess logic** (the `Game` and `Piece` classes) and **I/O** (the main loop). The notes below describe the design decisions that carry the most weight.

### Polymorphic piece hierarchy

`Piece` is an abstract base class declaring two pure virtual methods — `isLegalMove(...)` and `symbol()`. Each concrete type (`King`, `Queen`, `Rook`, `Bishop`, `Knight`, `Pawn`) inherits from it and supplies its own implementation. Because the board holds `Piece*` pointers, move validation is a single virtual call dispatched to the right piece at runtime; the game logic never switches on piece type.

Two further virtual methods, `isKing()` and `isPawn()`, return `false` by default and are overridden only where relevant. They let the game identify those pieces for check detection and en passant without downcasting, keeping the hierarchy's encapsulation intact. Shared movement logic that several pieces need — the `isPathClear()` check for sliding pieces — lives as a `protected` helper on the base class rather than being duplicated. Virtual destructors ensure pieces allocated as `Piece*` are destroyed correctly.

### Board representation

The board is a flat `Piece* board[8][8]`, indexed by column and row in `0..7`, with `nullptr` for empty squares. This keeps indexing, iteration, and — importantly — copying the board constant-time and straightforward. Copying matters because check detection works by **simulating** a proposed move on a temporary board copy and testing the result, so the live game state is never corrupted by an illegal trial move.

### Separation of concerns

`Game` (in `jeu.h` / `jeu.cpp`) owns the board, the game state, and every rule: move execution, castling, promotion, check and attack queries, and game-over evaluation. `main.cpp` owns everything else — reading moves from the terminal or a file, validating their syntax with regular expressions, and driving the loop. The `Game` class never touches input; `main.cpp` never reasons about chess. The same `tryMove` entry point handles both standard moves and castling, so the caller doesn't special-case them.

### Coordinate handling and state

All coordinate parsing funnels through one private `parseSquare` method, which converts algebraic names like `"e4"` into `(column, row)` index pairs (aliased as `Case = std::pair<int,int>`). Centralising this removes a whole class of off-by-one bugs from scattered character arithmetic.

Rule state that can't be derived from the board alone is tracked explicitly: `hasMoved` flags on `King` and `Rook` gate castling, `enPassantCol`/`enPassantRow` remember the one square where en passant is briefly legal, a half-move clock drives the fifty-move rule, and a vector of serialised board snapshots detects threefold repetition.

## Project structure

```
cpp-chess-engine/
├── src/
│   ├── pieces.h      # Abstract Piece base + concrete piece classes
│   ├── pieces.cpp    # Per-piece isLegalMove implementations
│   ├── jeu.h         # Game class: board, state, and rule declarations
│   ├── jeu.cpp       # Move execution, check/mate/draw logic, display
│   ├── main.cpp      # Input loop, regex validation, file replay
│   └── Makefile      # Builds src/chess
├── data/             # 42 recorded test games (legal and illegal), by level
├── test/
│   └── test-level.sh # Replays games and checks final position + result
└── README.md
```

> `jeu` and `echecs` are French for "game" and "chess" — a nod to the course's French-Azerbaijani origin; the filenames are kept as the team wrote them.

## Course context

Developed as a group mini-project for an Object-Oriented Programming course at UFAZ (French-Azerbaijani University). It exercises the core OOP toolkit in C++: an abstract base class with pure virtual methods, inheritance, runtime polymorphism via virtual dispatch, encapsulation of state behind a clean public interface, and RAII-style resource management with virtual destructors.
