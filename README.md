# OOP-SEMS-PROJECT
# ♟️ Chess Game with SFML & Audio

![C++](https://img.shields.io/badge/Language-C%2B%2B17-blue?style=flat-square&logo=cplusplus)
![SFML](https://img.shields.io/badge/Library-SFML%202.5+-green?style=flat-square)
![OOP](https://img.shields.io/badge/Course-Object--Oriented%20Programming-orange?style=flat-square)
![FAST](https://img.shields.io/badge/University-FAST--NUCES%20CFD-red?style=flat-square)

A fully functional two-player Chess game built in **C++** using **SFML** for graphics and audio. Developed as an OOP course project at FAST-NUCES CFD, Section 2C.

---

## 👥 Group Members

| Name | Roll Number | Role |
|------|-------------|------|
| Rana Riasat Ali Khan | 25F-0642 | Lead Developer + SFML Integrator|
| Sameen Khasif | 25F-0633 | Board Logic Implementor |
| Muhammad Bilal | 25F-0673 | Game Design Developer |

**University:** FAST-NUCES, CFD Campus  
**Section:** 2C

---

## 📁 File Structure

```
Chess/
├── CHESS.h       # Abstract base class + all 6 piece subclasses
├── CHESS.cpp     # Board class — game logic, move validation, special moves
├── main.cpp      # SFML window, render loop, audio, player input
├── move.ogg      # Sound effect for moves
└── capture.ogg   # Sound effect for captures
```

---

## 🧱 OOP Concepts Applied

### Inheritance
Base class `pieces` (abstract) is inherited by all six piece types — `Pawn`, `Rook`, `Knight`, `Bishop`, `Queen`, `King`. Each subclass overrides the pure virtual methods.

### Polymorphism
The board stores `pieces*` pointers. `isValidMove()` and `getSymbol()` are called virtually at runtime. `dynamic_cast<>` is used for type-specific logic (castling, en passant, promotion).

### Encapsulation
Piece data (`colour`, `rows`, `cols`) is `protected`. The `Board` class manages the grid privately. All state changes go through public methods like `movePiece()`, `castling()`, `isInCheck()`.

### Abstraction
`pieces` declares two pure virtual functions making it an abstract class — no `pieces` object can be instantiated directly.

---

## ♟️ Special Moves

### 1. En Passant
Tracked via `lastDoubleRow` and `lastDoubleCol` in the `Board` class.

**Conditions:**
- White pawn on row 3 (Black on row 4)
- Adjacent enemy pawn just moved two squares in the previous turn
- Capturing pawn moves diagonally to the square behind the captured pawn
- Captured pawn is deleted from the board after the move

**Location:** `Pawn::isValidMove()` + `Board::movePiece()` in `CHESS.cpp`

---

### 2. Castling
Implemented in `Board::castling(colour, side)` — supports both **Kingside** and **Queenside** for both colors.

**Conditions checked:**
- King has not moved (`hasMoved == false`)
- Chosen Rook has not moved (`hasMoved == false`)
- King is not currently in check
- All squares between King and Rook are empty

**Result:**
| Side | King moves to | Rook moves to |
|------|--------------|---------------|
| Kingside (right) | col 6 | col 5 |
| Queenside (left) | col 2 | col 3 |

> ⚠️ **Known limitation:** Does not verify that the King passes through or lands on an attacked square.

---

### 3. Pawn Promotion
When a pawn reaches the opposite end, it is automatically promoted to a **Queen**.

- White pawn reaches row `0` → replaced with `new Queen("W", toRow, toCol)`
- Black pawn reaches row `7` → replaced with `new Queen("B", toRow, toCol)`
- Original `Pawn` is deleted and a new `Queen` is heap-allocated

**Location:** `Board::movePiece()` in `CHESS.cpp`

---

### 4. Check Detection
`Board::isInCheck(colour)` finds the King's position, then iterates over all opponent pieces to see if any can attack the King via `isValidMove()`. A warning is shown to the player when their King is in check.

---

### 5. Checkmate & Stalemate
`Board::isStalemate(colour)` iterates all pieces of the given colour and tries every possible destination square (0–7, 0–7). If no valid move exists for any piece, it returns `true`.

---

## 🎮 Features

### Gameplay
- ✅ Full two-player local chess on 8×8 board
- ✅ Turn-based system with player switching
- ✅ All standard piece movements
- ✅ En passant, castling (both sides), pawn promotion
- ✅ Check warning
- ✅ Checkmate & stalemate detection
- ✅ 50-move draw rule
- ✅ Only-kings-left draw condition

### SFML Graphics
- ✅ Rendered board with alternating tile colors
- ✅ Piece symbols on tiles
- ✅ Move highlighting for selected piece
- ✅ Board labels (a–h, 1–8)
- ✅ Turn indicator

### Audio
- ✅ Move sound (`move.ogg`) on every legal move
- ✅ Capture sound (`capture.ogg`) when a piece is taken
- ✅ Implemented using `sf::SoundBuffer` and `sf::Sound`

---

## 🔧 Build & Run

### Requirements
- C++17 or later
- SFML 2.5+ (Graphics, Audio, Window, System)
- `g++` or MSVC compiler

### Compile
```bash
g++ main.cpp CHESS.cpp -o chess -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
```

### Run
```bash
./chess
```

> Make sure `move.ogg` and `capture.ogg` are in the same directory as the executable.

---

## ⚠️ Known Limitations

- Castling does not verify the King passes through attacked squares
- Pawn promotion is fixed to Queen (no underpromotion)
- No AI opponent — local two-player only

---

*Submitted for OOP Course | FAST-NUCES CFD | Section 2C*
