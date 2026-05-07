#include "chess.h"

// ── Constants ────────────────────────────────────────────────────────────────

const sf::Color HIGHLIGHT(130, 151, 105, 180);
const sf::Color CHECK_COLOR(255, 0, 0, 150);
const sf::Color VALID_MOVE_COLOR(100, 180, 100, 120);

const int TILE = 80;
const int OFFSET = 20;
const int PANEL_X = OFFSET + 8 * TILE + 20;

// ═══════════════════════════════════════════════════════════════════
// SOUND MANAGER
// ═══════════════════════════════════════════════════════════════════

SoundManager::SoundManager()
{
	bool ok = true;

	ok &= moveBuffer.loadFromFile("sounds/move.ogg");
	ok &= captureBuffer.loadFromFile("sounds/capture.ogg");
	ok &= checkBuffer.loadFromFile("sounds/check.ogg");
	ok &= gameoverBuffer.loadFromFile("sounds/gameover.ogg");
	ok &= castleBuffer.loadFromFile("sounds/castle.ogg");

	if (!ok)
	{
		cout << "Warning: Kuch sound files nahi mili, game bina sound ke chalega." << endl;
	}

	moveSound = new sf::Sound(moveBuffer);
	captureSound = new sf::Sound(captureBuffer);
	checkSound = new sf::Sound(checkBuffer);
	gameoverSound = new sf::Sound(gameoverBuffer);
	castleSound = new sf::Sound(castleBuffer);
}

SoundManager::~SoundManager()
{
	delete moveSound;
	delete captureSound;
	delete checkSound;
	delete gameoverSound;
	delete castleSound;
}

void SoundManager::playMove() { if (moveSound) { moveSound->play(); } }
void SoundManager::playCapture() { if (captureSound) { captureSound->play(); } }
void SoundManager::playCheck() { if (checkSound) { checkSound->play(); } }
void SoundManager::playGameover() { if (gameoverSound) { gameoverSound->play(); } }
void SoundManager::playCastle() { if (castleSound) { castleSound->play(); } }

// ═══════════════════════════════════════════════════════════════════
// BASE CLASS — pieces
// ═══════════════════════════════════════════════════════════════════

pieces::pieces() : colour(" "), rows(0), cols(0) {}

pieces::pieces(string colour, int rows, int cols) : colour(colour), rows(rows), cols(cols) {}

string pieces::getColour() const 
{
	return colour; 
}
int pieces::getRow() const 
{
	return rows; 
}
int pieces::getCol() const 
{
	return cols; 
}

void pieces::setPosition(int r, int c)
{
	rows = r;
	cols = c;
}


// ═══════════════════════════════════════════════════════════════════
// PAWN
// ═══════════════════════════════════════════════════════════════════

Pawn::Pawn(string colour, int rows, int cols) : pieces(colour, rows, cols) {}

string Pawn::getSymbol()
{
	return (colour == "W") ? "P" : "p";
}

bool Pawn::isValidMove(int toRow, int toCol, pieces* board[8][8],
	int lastDRow, int lastDCol)
{
	int fromRow = rows;
	int fromCol = cols;
	int dir = (colour == "W") ? -1 : 1;
	string enemy = (colour == "W") ? "B" : "W";

	// Aage ek square
	if (toCol == fromCol && toRow == fromRow + dir && board[toRow][toCol] == nullptr)
	{
		return true;
	}

	// Aage do square (starting position se)
	if (toCol == fromCol && toRow == fromRow + 2 * dir)
	{
		bool onStartRank = (colour == "W" && fromRow == 6) ||
			(colour == "B" && fromRow == 1);

		if (onStartRank &&
			board[fromRow + dir][fromCol] == nullptr &&
			board[toRow][toCol] == nullptr)
		{
			return true;
		}
	}

	// Diagonal capture
if (toRow == fromRow + dir && abs(toCol - fromCol) == 1)
{
	if (board[toRow][toCol] && board[toRow][toCol]->getColour() == enemy)
	{
		return true;
	}
	if (lastDRow == fromRow && lastDCol == toCol)
	{
		if (board[fromRow][toCol] != nullptr &&
			dynamic_cast<Pawn*>(board[fromRow][toCol]) &&
			board[fromRow][toCol]->getColour() == enemy)
		{
			return true;
		}
	}
}
	return false;
}

// ═══════════════════════════════════════════════════════════════════
// ROOK
// ═══════════════════════════════════════════════════════════════════

Rook::Rook(string colour, int rows, int cols) : pieces(colour, rows, cols) {}

string Rook::getSymbol()
{
	return (colour == "W") ? "R" : "r";
}

void Rook::setHasMoved() { hasMoved = true; }
bool Rook::getHasMoved() const { return hasMoved; }

bool Rook::isValidMove(int toRow, int toCol, pieces* board[8][8],
	int lastDRow, int lastDCol)
{
	int fromRow = rows;
	int fromCol = cols;

	if (toRow != fromRow && toCol != fromCol)
	{
		return false;
	}

	if (toRow == fromRow)
	{
		int step = (toCol > fromCol) ? 1 : -1;

		for (int j = fromCol + step; j != toCol; j += step)
		{
			if (board[fromRow][j]) { return false; }
		}
	}
	else
	{
		int step = (toRow > fromRow) ? 1 : -1;

		for (int i = fromRow + step; i != toRow; i += step)
		{
			if (board[i][fromCol]) { return false; }
		}
	}

	if (board[toRow][toCol] && board[toRow][toCol]->getColour() == colour)
	{
		return false;
	}

	return true;
}

// ═══════════════════════════════════════════════════════════════════
// KNIGHT
// ═══════════════════════════════════════════════════════════════════

Knight::Knight(string colour, int rows, int cols) : pieces(colour, rows, cols) {}

string Knight::getSymbol()
{
	return (colour == "W") ? "N" : "n";
}

bool Knight::isValidMove(int toRow, int toCol, pieces* board[8][8],
	int lastDRow, int lastDCol)
{
	int rD = abs(toRow - rows);
	int cD = abs(toCol - cols);

	if (!((rD == 2 && cD == 1) || (rD == 1 && cD == 2)))
	{
		return false;
	}

	if (board[toRow][toCol] && board[toRow][toCol]->getColour() == colour)
	{
		return false;
	}

	return true;
}

// ═══════════════════════════════════════════════════════════════════
// BISHOP
// ═══════════════════════════════════════════════════════════════════

Bishop::Bishop(string colour, int rows, int cols) : pieces(colour, rows, cols) {}

string Bishop::getSymbol()
{
	return (colour == "W") ? "B" : "b";
}

bool Bishop::isValidMove(int toRow, int toCol, pieces* board[8][8],
	int lastDRow, int lastDCol)
{
	int fromRow = rows;
	int fromCol = cols;

	if (abs(toRow - fromRow) != abs(toCol - fromCol))
	{
		return false;
	}

	int rStep = (toRow > fromRow) ? 1 : -1;
	int cStep = (toCol > fromCol) ? 1 : -1;
	int r = fromRow + rStep;
	int c = fromCol + cStep;

	while (r != toRow)
	{
		if (board[r][c]) { return false; }
		r += rStep;
		c += cStep;
	}

	if (board[toRow][toCol] && board[toRow][toCol]->getColour() == colour)
	{
		return false;
	}

	return true;
}

// ═══════════════════════════════════════════════════════════════════
// QUEEN
// ═══════════════════════════════════════════════════════════════════

Queen::Queen(string colour, int rows, int cols) : pieces(colour, rows, cols) {}

string Queen::getSymbol()
{
	return (colour == "W") ? "Q" : "q";
}

bool Queen::isValidMove(int toRow, int toCol, pieces* board[8][8],
	int lastDRow, int lastDCol)
{
	int fromRow = rows;
	int fromCol = cols;
	int rD = abs(toRow - fromRow);
	int cD = abs(toCol - fromCol);
if (toRow == fromRow || toCol == fromCol)
{
	if (toRow == fromRow)
	{
		int step = (toCol > fromCol) ? 1 : -1;

		for (int j = fromCol + step; j != toCol; j += step)
		{
			if (board[fromRow][j]) 
			{
				return false; 
			}
		}
	}
	else
	{
		int step = (toRow > fromRow) ? 1 : -1;

		for (int i = fromRow + step; i != toRow; i += step)
		{
			if (board[i][fromCol])
			{
				return false;
			}
		}
	}
}

	// Bishop jaisi movement (diagonal)
	else if (rD == cD)
	{
		int rStep = (toRow > fromRow) ? 1 : -1;
		int cStep = (toCol > fromCol) ? 1 : -1;
		int r = fromRow + rStep;
		int c = fromCol + cStep;

		while (r != toRow)
		{
			if (board[r][c]) { return false; }
			r += rStep;
			c += cStep;
		}
	}
	else
	{
		return false;
	}

	if (board[toRow][toCol] && board[toRow][toCol]->getColour() == colour)
	{
		return false;
	}

	return true;
}

// ═══════════════════════════════════════════════════════════════════
// KING
// ═══════════════════════════════════════════════════════════════════

King::King(string colour, int rows, int cols) : pieces(colour, rows, cols) {}

string King::getSymbol()
{
	return (colour == "W") ? "K" : "k";
}

void King::setHasMoved() 
{
	hasMoved = true; 
}
bool King::getHasMoved() const
{
	return hasMoved; 
}


bool King::isValidMove(int toRow, int toCol, pieces* board[8][8],
	int lastDRow, int lastDCol)
{
	if (abs(toRow - rows) > 1 || abs(toCol - cols) > 1)
	{
		return false;
	}

	if (board[toRow][toCol] && board[toRow][toCol]->getColour() == colour)
	{
		return false;
	}

	return true;
}

// ═══════════════════════════════════════════════════════════════════
// BOARD
// ═══════════════════════════════════════════════════════════════════

Board::Board()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			grid[i][j] = nullptr;
		}
	}

	setupPieces();
}

Board::~Board()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			delete grid[i][j];
			grid[i][j] = nullptr;
		}
	}
}

pieces* Board::getGrid(int r, int c) const
{
	return grid[r][c];
}
string Board::getCurrentPlayer() const
{
	return currentPlayer;
}
int Board::getMoveCount() const
{
	return moveCount;
}
bool Board::wasCapture() const
{
	return lastMoveWasCapture;
}

// ── Setup ────────────────────────────────────────────────────

void Board::setupPieces()
{
	// Black back rank
	grid[0][0] = new Rook("B", 0, 0);
	grid[0][1] = new Knight("B", 0, 1);
	grid[0][2] = new Bishop("B", 0, 2);
	grid[0][3] = new Queen("B", 0, 3);
	grid[0][4] = new King("B", 0, 4);
	grid[0][5] = new Bishop("B", 0, 5);
	grid[0][6] = new Knight("B", 0, 6);
	grid[0][7] = new Rook("B", 0, 7);

	for (int j = 0; j < 8; j++)
	{
		grid[1][j] = new Pawn("B", 1, j);
	}

	// White back rank
	grid[7][0] = new Rook("W", 7, 0);
	grid[7][1] = new Knight("W", 7, 1);
	grid[7][2] = new Bishop("W", 7, 2);
	grid[7][3] = new Queen("W", 7, 3);
	grid[7][4] = new King("W", 7, 4);
	grid[7][5] = new Bishop("W", 7, 5);
	grid[7][6] = new Knight("W", 7, 6);
	grid[7][7] = new Rook("W", 7, 7);

	for (int j = 0; j < 8; j++)
	{
		grid[6][j] = new Pawn("W", 6, j);
	}

	// Middle squares empty
	for (int i = 2; i <= 5; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			grid[i][j] = nullptr;
		}
	}
}

// ── Check simulation (move undo) ─────────────────────────────

bool Board::tryMoveAndCheck(pieces* piece, int fromRow, int fromCol, int toRow, int toCol)
{
	pieces* captured = grid[toRow][toCol];
	pieces* epCaptured = nullptr;
	int epRow = -1;
	int epCol = -1;

	// En passant: captured pawn alag square pe hota hai
	if (dynamic_cast<Pawn*>(piece) && toCol != fromCol && captured == nullptr)
	{
		epRow = fromRow;
		epCol = toCol;
		epCaptured = grid[epRow][epCol];
		grid[epRow][epCol] = nullptr;
	}

	// Move simulate karo
	grid[toRow][toCol] = piece;
	grid[fromRow][fromCol] = nullptr;

	int oldR = piece->getRow();
	int oldC = piece->getCol();
	piece->setPosition(toRow, toCol);

	bool inCheck = isInCheck(piece->getColour());

	// Undo
	grid[fromRow][fromCol] = piece;
	grid[toRow][toCol] = captured;
	piece->setPosition(oldR, oldC);

	if (epCaptured)
	{
		grid[epRow][epCol] = epCaptured;
	}

	return !inCheck;
}

// ── Move karo ────────────────────────────────────────────────

bool Board::movePiece(int fromRow, int fromCol, int toRow, int toCol)
{
	if (!grid[fromRow][fromCol])
	{
		return false;
	}

	pieces* piece = grid[fromRow][fromCol];

	if (piece->getColour() != currentPlayer)
	{
		return false;
	}

	if (!piece->isValidMove(toRow, toCol, grid, lastDoubleRow, lastDoubleCol))
	{
		return false;
	}

	if (!tryMoveAndCheck(piece, fromRow, fromCol, toRow, toCol))
	{
		return false;
	}

	bool isEnPassant = (dynamic_cast<Pawn*>(piece) && toCol != fromCol && grid[toRow][toCol] == nullptr);

	// Capture flag set karo (sound ke liye)
lastMoveWasCapture = (grid[toRow][toCol] != nullptr) || isEnPassant;

	// Move execute karo
	pieces* captured = grid[toRow][toCol];
	grid[toRow][toCol] = piece;
	grid[fromRow][fromCol] = nullptr;
	piece->setPosition(toRow, toCol);

	if (captured)
	{
		delete captured;
	}

	// En passant pawn hata do
	if (isEnPassant && grid[fromRow][toCol] &&
		dynamic_cast<Pawn*>(grid[fromRow][toCol]))
	{
		delete grid[fromRow][toCol];
		grid[fromRow][toCol] = nullptr;
	}

	// En passant tracker update karo

	if (dynamic_cast<Pawn*>(piece) && abs(toRow - fromRow) == 2)
	{
		lastDoubleRow = toRow;
		lastDoubleCol = toCol;
	}
	else
	{
		lastDoubleRow = -1;
		lastDoubleCol = -1;
	}
	
	// Pawn promotion (auto queen)
	if (dynamic_cast<Pawn*>(piece))
	{
		if (piece->getColour() == "W" && toRow == 0)
		{
			delete grid[toRow][toCol];
			grid[toRow][toCol] = new Queen("W", toRow, toCol);
		}
		else if (piece->getColour() == "B" && toRow == 7)
		{
			delete grid[toRow][toCol];
			grid[toRow][toCol] = new Queen("B", toRow, toCol);
		}
	}

}

// ── Player switch ─────────────────────────────────────────────

void Board::switchPlayer()
{
	currentPlayer = (currentPlayer == "W") ? "B" : "W";
}

// ── Check detection ──────────────────────────────────────────

bool Board::isInCheck(string colour)
{
	int kingRow = -1;
	int kingCol = -1;

	for (int i = 0; i < 8 && kingRow == -1; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (grid[i][j] &&
				grid[i][j]->getColour() == colour &&
				dynamic_cast<King*>(grid[i][j]))
			{
				kingRow = i;
				kingCol = j;
				break;
			}
		}
	}

	if (kingRow == -1) { return false; }

	string opp = (colour == "W") ? "B" : "W";

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (grid[i][j] && grid[i][j]->getColour() == opp)
			{
				if (grid[i][j]->isValidMove(kingRow, kingCol, grid))
				{
					return true;
				}
			}
		}
	}

	return false;
}

// ── Checkmate / Stalemate ────────────────────────────────────

bool Board::isCheckmate(string colour)
{
	if (!isInCheck(colour)) { return false; }
	return !hasAnyLegalMove(colour);
}

bool Board::isStalemate(string colour)
{
	if (isInCheck(colour)) { return false; }
	return !hasAnyLegalMove(colour);
}

bool Board::hasAnyLegalMove(string colour)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (grid[i][j] && grid[i][j]->getColour() == colour)
			{
				for (int r = 0; r < 8; r++)
				{
					for (int c = 0; c < 8; c++)
					{
						if (grid[i][j]->isValidMove(r, c, grid, lastDoubleRow, lastDoubleCol))
						{
							if (tryMoveAndCheck(grid[i][j], i, j, r, c))
							{
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

// ── Valid moves highlight ke liye ────────────────────────────

void Board::getValidMoves(int fromRow, int fromCol, bool validMoves[8][8])
{
	for (int r = 0; r < 8; r++)
	{
		for (int c = 0; c < 8; c++)
		{
			validMoves[r][c] = false;
		}
	}

	if (!grid[fromRow][fromCol]) { return; }

	pieces* piece = grid[fromRow][fromCol];

	for (int r = 0; r < 8; r++)
	{
		for (int c = 0; c < 8; c++)
		{
			if (piece->isValidMove(r, c, grid, lastDoubleRow, lastDoubleCol))
			{
				if (tryMoveAndCheck(piece, fromRow, fromCol, r, c))
				{
					validMoves[r][c] = true;
				}
			}
		}
	}
}

// ── Misc helpers ─────────────────────────────────────────────

bool Board::isKingAlive(string colour)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (grid[i][j] &&
				grid[i][j]->getColour() == colour &&
				dynamic_cast<King*>(grid[i][j]))
			{
				return true;
			}
		}
	}

	return false;
}

bool Board::onlyKingsLeft()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (grid[i][j] && !dynamic_cast<King*>(grid[i][j]))
			{
				return false;
			}
		}
	}

	return true;
}

// ── Castling ─────────────────────────────────────────────────

bool Board::castling(string colour, string side)
{
	int row = (colour == "W") ? 7 : 0;
	King* king = dynamic_cast<King*>(grid[row][4]);

	if (!king || king->getHasMoved()) { return false; }
	if (isInCheck(colour)) { return false; }

	int rookCol = (side == "right") ? 7 : 0;
	int step = (side == "right") ? 1 : -1;
	Rook* rook = dynamic_cast<Rook*>(grid[row][rookCol]);

	if (!rook || rook->getHasMoved()) { return false; }

	// Path empty hona chahiye
	for (int c = 4 + step; c != rookCol; c += step)
	{
		if (grid[row][c]) { return false; }
	}

	// King ka path check-free hona chahiye
	for (int step2 = 1; step2 <= 2; step2++)
	{
		int col = 4 + step * step2;

		if (col < 0 || col > 7) { break; }

		if (!tryMoveAndCheck(king, row, 4, row, col))
		{
			return false;
		}
	}

	// Castling execute karo
	int kingDest = 4 + 2 * step;
	int rookDest = 4 + step;

	grid[row][4] = nullptr;
	grid[row][kingDest] = king;
	king->setPosition(row, kingDest);
	king->setHasMoved();

	grid[row][rookCol] = nullptr;
	grid[row][rookDest] = rook;
	rook->setPosition(row, rookDest);
	rook->setHasMoved();

	moveCount++;
	return true;
}

// ── King position dhundo ─────────────────────────────────────

void Board::getKingPosition(string colour, int& kr, int& kc)
{
	kr = -1;
	kc = -1;

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (grid[i][j] && grid[i][j]->getColour() == colour && dynamic_cast<King*>(grid[i][j]))
			{
				kr = i;
				kc = j;
				return;
			}
		}
	}
}

// ═══════════════════════════════════════════════════════════════════
// DRAW — SIDE PANEL
// ═══════════════════════════════════════════════════════════════════

void drawSidePanel(sf::RenderWindow& window, sf::Font& font,
	const string& currentPlayer, const string& statusMsg,
	int moveCount, bool gameOver)
{
	// Panel background
	sf::RectangleShape panel(sf::Vector2f(160, 680));
	panel.setPosition(sf::Vector2f(PANEL_X - 10, OFFSET));
	panel.setFillColor(sf::Color(20, 20, 20, 230));
	panel.setOutlineColor(sf::Color(80, 80, 80));
	panel.setOutlineThickness(1);
	window.draw(panel);

	int y = OFFSET + 15;

	auto drawDivider = [&](int yPos)
		{
			sf::RectangleShape div(sf::Vector2f(140, 1));
			div.setPosition(sf::Vector2f(PANEL_X - 5, yPos));
			div.setFillColor(sf::Color(80, 80, 80));
			window.draw(div);
		};

	// Title
	sf::Text title(font, "CHESS", 22);
	title.setFillColor(sf::Color(200, 180, 120));
	title.setStyle(sf::Text::Bold);
	title.setPosition(sf::Vector2f(PANEL_X + 18, y));
	window.draw(title);
	y += 35;

	drawDivider(y);
	y += 15;

	// Turn indicator (sirf game active mein)
	if (!gameOver)
	{
		sf::Text turnLabel(font, "TURN", 11);
		turnLabel.setFillColor(sf::Color(120, 120, 120));
		turnLabel.setStyle(sf::Text::Bold);
		turnLabel.setPosition(sf::Vector2f(PANEL_X + 5, y));
		window.draw(turnLabel);
		y += 20;

		sf::CircleShape circle(18);
		circle.setFillColor((currentPlayer == "W")
			? sf::Color(240, 235, 220) : sf::Color(40, 40, 40));
		circle.setOutlineColor(sf::Color(200, 180, 120));
		circle.setOutlineThickness(2);
		circle.setPosition(sf::Vector2f(PANEL_X + 5, y));
		window.draw(circle);

		sf::Text playerName(font, (currentPlayer == "W") ? "WHITE" : "BLACK", 14);
		playerName.setFillColor((currentPlayer == "W")
			? sf::Color(240, 235, 220) : sf::Color(160, 160, 160));
		playerName.setStyle(sf::Text::Bold);
		playerName.setPosition(sf::Vector2f(PANEL_X + 50, y + 8));
		window.draw(playerName);
		y += 55;

		drawDivider(y);
		y += 15;
	}

	// Move counter
	sf::Text movesLabel(font, "MOVES", 11);
	movesLabel.setFillColor(sf::Color(120, 120, 120));
	movesLabel.setStyle(sf::Text::Bold);
	movesLabel.setPosition(sf::Vector2f(PANEL_X + 5, y));
	window.draw(movesLabel);
	y += 20;

	sf::Text movesVal(font, to_string(moveCount), 28);
	movesVal.setFillColor(sf::Color(200, 180, 120));
	movesVal.setStyle(sf::Text::Bold);
	movesVal.setPosition(sf::Vector2f(PANEL_X + 10, y));
	window.draw(movesVal);
	y += 50;

	drawDivider(y);
	y += 15;

	// Status message
	sf::Text statusLabel(font, "STATUS", 11);
	statusLabel.setFillColor(sf::Color(120, 120, 120));
	statusLabel.setStyle(sf::Text::Bold);
	statusLabel.setPosition(sf::Vector2f(PANEL_X + 5, y));
	window.draw(statusLabel);
	y += 20;

	sf::Color statusColor = sf::Color(180, 180, 180);

	if (statusMsg.find("CHECK") != string::npos) { statusColor = sf::Color(255, 100, 100); }
	else if (statusMsg.find("WINS") != string::npos) { statusColor = sf::Color(100, 220, 100); }
	else if (statusMsg.find("DRAW") != string::npos) { statusColor = sf::Color(255, 200, 50); }
	else if (statusMsg.find("Invalid") != string::npos) { statusColor = sf::Color(255, 120, 80); }

	// Word wrap
	string line = "";
	int lineLen = 0;

	for (int i = 0; i < (int)statusMsg.size(); i++)
	{
		line += statusMsg[i];
		lineLen++;

		if (lineLen >= 14 && statusMsg[i] == ' ')
		{
			sf::Text st(font, line, 13);
			st.setFillColor(statusColor);
			st.setPosition(sf::Vector2f(PANEL_X + 5, y));
			window.draw(st);
			y += 18;
			line = "";
			lineLen = 0;
		}
	}

	if (!line.empty())
	{
		sf::Text st(font, line, 13);
		st.setFillColor(statusColor);
		st.setPosition(sf::Vector2f(PANEL_X + 5, y));
		window.draw(st);
		y += 18;
	}

	y += 10;
	drawDivider(y);
	y += 15;

	// Hint
	sf::Text hint(font, "Press C\nto castle", 11);
	hint.setFillColor(sf::Color(90, 90, 90));
	hint.setPosition(sf::Vector2f(PANEL_X + 5, y));
	window.draw(hint);
}

// ═══════════════════════════════════════════════════════════════════
// DRAW — GAME OVER OVERLAY
// ═══════════════════════════════════════════════════════════════════

void drawGameOverOverlay(sf::RenderWindow& window, sf::Font& font,
	const string& statusMsg)
{
	// Dark overlay
	sf::RectangleShape overlay(sf::Vector2f(800, 800));
	overlay.setFillColor(sf::Color(0, 0, 0, 160));
	window.draw(overlay);

	// Box
	sf::RectangleShape box(sf::Vector2f(400, 180));
	box.setFillColor(sf::Color(15, 15, 15, 240));
	box.setOutlineColor(sf::Color(200, 180, 120));
	box.setOutlineThickness(2);
	box.setPosition(sf::Vector2f(200, 290));
	window.draw(box);

	// Result text
	string icon;
	sf::Color iconColor;

	if (statusMsg.find("WHITE WINS") != string::npos) { icon = "WHITE WINS"; iconColor = sf::Color(240, 235, 210); }
	else if (statusMsg.find("BLACK WINS") != string::npos) { icon = "BLACK WINS"; iconColor = sf::Color(160, 160, 160); }
	else { icon = "DRAW"; iconColor = sf::Color(255, 200, 60); }

	sf::Text bigText(font, icon, 38);
	bigText.setFillColor(iconColor);
	bigText.setStyle(sf::Text::Bold);
	sf::FloatRect br = bigText.getLocalBounds();
	bigText.setPosition(sf::Vector2f(400 - br.size.x / 2, 310));
	window.draw(bigText);

	sf::Text subText(font, statusMsg, 16);
	subText.setFillColor(sf::Color(180, 180, 180));
	sf::FloatRect sr = subText.getLocalBounds();
	subText.setPosition(sf::Vector2f(400 - sr.size.x / 2, 375));
	window.draw(subText);

	sf::Text escHint(font, "Press ESC to close", 13);
	escHint.setFillColor(sf::Color(80, 80, 80));
	sf::FloatRect er = escHint.getLocalBounds();
	escHint.setPosition(sf::Vector2f(400 - er.size.x / 2, 430));
	window.draw(escHint);
}

// ═══════════════════════════════════════════════════════════════════
// DRAW — HIGHLIGHTS
// ═══════════════════════════════════════════════════════════════════

void drawHighlights(sf::RenderWindow& window,
	int selRow, int selCol,
	bool validMoves[8][8],
	bool inCheck, int kingRow, int kingCol)
{
	// Selected piece highlight
	if (selRow != -1)
	{
		sf::RectangleShape sq(sf::Vector2f(80, 80));
		sq.setPosition(sf::Vector2f(OFFSET + selCol * TILE, OFFSET + selRow * TILE));
		sq.setFillColor(HIGHLIGHT);
		window.draw(sq);

		// Valid move dots
		for (int r = 0; r < 8; r++)
		{
			for (int c = 0; c < 8; c++)
			{
				if (!validMoves[r][c]) { continue; }

				float cx = OFFSET + c * TILE + TILE / 2.0f;
				float cy = OFFSET + r * TILE + TILE / 2.0f;

				sf::CircleShape dot(10);
				dot.setFillColor(VALID_MOVE_COLOR);
				dot.setOrigin(sf::Vector2f(10, 10));
				dot.setPosition(sf::Vector2f(cx, cy));
				window.draw(dot);
			}
		}
	}

	// King in check highlight
	if (inCheck && kingRow != -1)
	{
		sf::RectangleShape sq(sf::Vector2f(80, 80));
		sq.setPosition(sf::Vector2f(OFFSET + kingCol * TILE, OFFSET + kingRow * TILE));
		sq.setFillColor(CHECK_COLOR);
		window.draw(sq);
	}
}

// ═══════════════════════════════════════════════════════════════════
// DRAW — PIECE
// ═══════════════════════════════════════════════════════════════════

void drawPiece(sf::RenderWindow& window, map<string, sf::Texture>& textures,
	const string& symbol, int row, int col)
{
	const string symbols[12] = { "P","R","N","B","Q","K","p","r","n","b","q","k" };
	const string filenames[12] = {
	"white-pawn","white-rook","white-knight","white-bishop","white-queen","white-king",
	"black-pawn","black-rook","black-knight","black-bishop","black-queen","black-king"
	};

	string key = "";

	for (int i = 0; i < 12; i++)
	{
		if (symbols[i] == symbol)
		{
			key = filenames[i];
			break;
		}
	}

	if (key.empty()) { return; }

	sf::Sprite sprite(textures[key]);
	sf::Vector2u texSize = textures[key].getSize();
	sprite.setScale(sf::Vector2f((float)TILE / texSize.x, (float)TILE / texSize.y));
	sprite.setPosition(sf::Vector2f(OFFSET + col * TILE, OFFSET + row * TILE));
	window.draw(sprite);
}
