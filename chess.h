#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <map>
using namespace std;

//CONSTANT VARIABLES
extern const sf::Color HIGHLIGHT;
extern const sf::Color CHECK_COLOR;
extern const sf::Color VALID_MOVE_COLOR;
extern const int TILE;
extern const int OFFSET;
extern const int PANEL_X;

// SOUND MANAGER CLASS
class SoundManager
{
private:
	sf::SoundBuffer moveBuffer;
	sf::SoundBuffer captureBuffer;
	sf::SoundBuffer checkBuffer;
	sf::SoundBuffer gameoverBuffer;
	sf::SoundBuffer castleBuffer;

	sf::Sound* moveSound = nullptr;
	sf::Sound* captureSound = nullptr;
	sf::Sound* checkSound = nullptr;
	sf::Sound* gameoverSound = nullptr;
	sf::Sound* castleSound = nullptr;

public:
	SoundManager();
	~SoundManager();

	void playMove();
	void playCapture();
	void playCheck();
	void playGameover();
	void playCastle();
};

//PIECES ABSTRACT CLASS
class pieces
{
protected:
	string colour;
	int rows;
	int cols;

public:
	pieces();
	pieces(string colour, int rows, int cols);

	string getColour() const;
	int getRow() const;
	int getCol() const;

	void setPosition(int r, int c);

	virtual bool isValidMove(int toRow, int toCol, pieces* board[8][8],int lastDRow = -1, int lastDCol = -1) = 0;
	virtual string getSymbol() = 0;
	virtual ~pieces() {}
};

// ═══════════════════════════════════════════════════════════════════
// PAWN
// ═══════════════════════════════════════════════════════════════════

class Pawn : public pieces
{
public:
	Pawn(string colour, int rows, int cols);
	string getSymbol() override;
	bool isValidMove(int toRow, int toCol, pieces* board[8][8],int lastDRow = -1, int lastDCol = -1) override;
};

// ═══════════════════════════════════════════════════════════════════
// ROOK
// ═══════════════════════════════════════════════════════════════════

class Rook : public pieces
{
private:
	bool hasMoved = false;
public:
	Rook(string colour, int rows, int cols);

	string getSymbol() override;

	void setHasMoved();
	bool getHasMoved() const;

	bool isValidMove(int toRow, int toCol, pieces* board[8][8],int lastDRow = -1, int lastDCol = -1) override;
};

// ═══════════════════════════════════════════════════════════════════
// KNIGHT
// ═══════════════════════════════════════════════════════════════════

class Knight : public pieces
{
public:
	Knight(string colour, int rows, int cols);
	string getSymbol() override;
	bool isValidMove(int toRow, int toCol, pieces* board[8][8],int lastDRow = -1, int lastDCol = -1) override;
};

// ═══════════════════════════════════════════════════════════════════
// BISHOP
// ═══════════════════════════════════════════════════════════════════

class Bishop : public pieces
{
public:
	Bishop(string colour, int rows, int cols);string getSymbol() override;
	bool isValidMove(int toRow, int toCol, pieces* board[8][8],int lastDRow = -1, int lastDCol = -1) override;
};

// ═══════════════════════════════════════════════════════════════════
// QUEEN
// ═══════════════════════════════════════════════════════════════════

class Queen : public pieces
{
public:
	Queen(string colour, int rows, int cols);
	string getSymbol() override;
	bool isValidMove(int toRow, int toCol, pieces* board[8][8], int lastDRow = -1, int lastDCol = -1) override;
};

// ═══════════════════════════════════════════════════════════════════
// KING
// ═══════════════════════════════════════════════════════════════════

class King : public pieces
{
private:
	bool hasMoved = false;
public:
	King(string colour, int rows, int cols);
	string getSymbol() override;
	void setHasMoved();
	bool getHasMoved() const;
	bool isValidMove(int toRow, int toCol, pieces* board[8][8],int lastDRow = -1, int lastDCol = -1) override;
};

// ═══════════════════════════════════════════════════════════════════
// BOARD
// ═══════════════════════════════════════════════════════════════════

class Board
{
private:
	pieces* grid[8][8];
	string currentPlayer = "W";
	int moveCount = 0;
	int lastDoubleRow = -1;
	int lastDoubleCol = -1;
	bool lastMoveWasCapture = false;
public:
	Board();
	~Board();
	// Getters
	pieces* getGrid(int r, int c) const;
	string getCurrentPlayer() const;
	int getMoveCount() const;
	bool wasCapture() const;
	// Setup
	void setupPieces();
	// Check simulation (move undo)
	bool tryMoveAndCheck(pieces* piece, int fromRow, int fromCol, int toRow, int toCol);
	// Moved
	bool movePiece(int fromRow, int fromCol, int toRow, int toCol);
	// Player switch
	void switchPlayer();
	// Check detection
	bool isInCheck(string colour);
	// Checkmate / Stalemate
	bool isCheckmate(string colour);
	bool isStalemate(string colour);
	bool hasAnyLegalMove(string colour);
	// Valid moves highlighted
	void getValidMoves(int fromRow, int fromCol, bool validMoves[8][8]);
	// Misc helpers
	bool isKingAlive(string colour);
	bool onlyKingsLeft();
	// Castling
	bool castling(string colour, string side);
	// Finding King position 
	void getKingPosition(string colour, int& kr, int& kc);
};

// ═══════════════════════════════════════════════════════════════════
// FREE DRAW FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

void drawSidePanel(sf::RenderWindow& window, sf::Font& font,const string& currentPlayer, const string& statusMsg,int moveCount, bool gameOver);
void drawGameOverOverlay(sf::RenderWindow& window, sf::Font& font,const string& statusMsg);
void drawHighlights(sf::RenderWindow& window,int selRow, int selCol,bool validMoves[8][8],bool inCheck, int kingRow, int kingCol);
void drawPiece(sf::RenderWindow& window, sf::Texture textures[12],const string& symbol, int row, int col)
