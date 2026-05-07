#include "chess.h"

int main()
{
	sf::RenderWindow window(sf::VideoMode(sf::Vector2u(980, 700)), "Chess Game");
	window.setFramerateLimit(60);


	sf::Font font;

	if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
	{
		cout << "ERROR: Font not found!" << endl;
		return -1;
	}

	// ── Board texture ────────────────────────────────────────────

	sf::Texture boardTexture;

	if (!boardTexture.loadFromFile("board.png"))
	{
		cout << "ERROR: board.png not found!" << endl;
		return -1;
	}

	sf::Sprite boardSprite(boardTexture);
	sf::Vector2u boardSize = boardTexture.getSize();
	boardSprite.setScale(sf::Vector2f((float)(TILE * 8) / boardSize.x,
		(float)(TILE * 8) / boardSize.y));
	boardSprite.setPosition(sf::Vector2f(OFFSET, OFFSET));

	// ── Piece textures ───────────────────────────────────────────

	const string names[12] = {
	"white-pawn","white-rook","white-knight","white-bishop","white-queen","white-king",
	"black-pawn","black-rook","black-knight","black-bishop","black-queen","black-king"
	};

	map<string, sf::Texture> textures;

	for (const auto& name : names)
	{
		sf::Texture tex;

		if (!tex.loadFromFile("pieces/" + name + ".png"))
		{
			cout << "ERROR: pieces/" << name << ".png not found!" << endl;
			return -1;
		}

		textures[name] = move(tex);
	}

	// ── Sound ────────────────────────────────────────────────────

	SoundManager sounds;

	// ── Game state ───────────────────────────────────────────────

	Board board;
	int selRow = -1;
	int selCol = -1;
	bool selected = false;
	bool gameOver = false;
	string statusMsg = "White Player's Turn";
	bool validMoves[8][8] = {};

	cout << "Chess game started." << endl;

	// ═══════════════════════════════════════════════════════════════
	// GAME LOOP
	// ═══════════════════════════════════════════════════════════════

	while (window.isOpen())
	{
		// ── Events ───────────────────────────────────────────────

		while (const auto event = window.pollEvent())
		{
			// Window close
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}

			// ESC to close
			if (const auto* key = event->getIf<sf::Event::KeyPressed>())
			{
				if (key->code == sf::Keyboard::Key::Escape)
				{
					window.close();
				}
			}

			if (!gameOver)
			{
				// ── Mouse click ──────────────────────────────────

				if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
				{
					int col = (mouse->position.x - OFFSET) / TILE;
					int row = (mouse->position.y - OFFSET) / TILE;

					if (col >= 0 && col < 8 && row >= 0 && row < 8)
					{
						if (!selected)
						{
							// Apni piece select karo
							if (board.getGrid(row, col) &&
								board.getGrid(row, col)->getColour() == board.getCurrentPlayer())
							{
								selRow = row;
								selCol = col;
								selected = true;
								board.getValidMoves(row, col, validMoves);
							}
						}
						else
						{
							// Same square click — deselect
							if (row == selRow && col == selCol)
							{
								selected = false;
								selRow = -1;
								selCol = -1;
								memset(validMoves, 0, sizeof(validMoves));
							}
							// Dusri apni piece — reselect
							else if (board.getGrid(row, col) &&
								board.getGrid(row, col)->getColour() == board.getCurrentPlayer())
							{
								selRow = row;
								selCol = col;
								board.getValidMoves(row, col, validMoves);
							}
							// Move attempt
							else
							{
								if (board.movePiece(selRow, selCol, row, col))
								{
									board.switchPlayer();

									// Game over checks
									if (!board.isKingAlive("W"))
									{
										statusMsg = "BLACK WINS!";
										gameOver = true;
										sounds.playGameover();
									}
									else if (!board.isKingAlive("B"))
									{
										statusMsg = "WHITE WINS!";
										gameOver = true;
										sounds.playGameover();
									}
									else if (board.getMoveCount() >= 200)
									{
										statusMsg = "DRAW! Move limit reached!";
										gameOver = true;
										sounds.playGameover();
									}
									else if (board.onlyKingsLeft())
									{
										statusMsg = "DRAW! Only kings left!";
										gameOver = true;
										sounds.playGameover();
									}
									else if (board.isCheckmate(board.getCurrentPlayer()))
									{
										statusMsg = (board.getCurrentPlayer() == "W")
											? "BLACK WINS! CHECKMATE!"
											: "WHITE WINS! CHECKMATE!";
										gameOver = true;
										sounds.playGameover();
									}
									else if (board.isStalemate(board.getCurrentPlayer()))
									{
										statusMsg = "DRAW! Stalemate!";
										gameOver = true;
										sounds.playGameover();
									}
									else if (board.isInCheck(board.getCurrentPlayer()))
									{
										statusMsg = (board.getCurrentPlayer() == "W")
											? "WARNING! White King in CHECK!"
											: "WARNING! Black King in CHECK!";
										sounds.playCheck();
									}
									else
									{
										statusMsg = (board.getCurrentPlayer() == "W")
											? "White Player's Turn"
											: "Black Player's Turn";

										if (board.wasCapture())
										{
											sounds.playCapture();
										}
										else
										{
											sounds.playMove();
										}
									}
								}
								else
								{
									statusMsg = "Invalid Move! Try Again.";
								}

								selected = false;
								selRow = -1;
								selCol = -1;
								memset(validMoves, 0, sizeof(validMoves));
							}
						}
					}
				}

				// ── C key: Castling ──────────────────────────────

				if (const auto* key = event->getIf<sf::Event::KeyPressed>())
				{
					if (key->code == sf::Keyboard::Key::C)
					{
						bool castled = board.castling(board.getCurrentPlayer(), "right");

						if (!castled)
						{
							castled = board.castling(board.getCurrentPlayer(), "left");
						}

						if (castled)
						{
							board.switchPlayer();
							statusMsg = (board.getCurrentPlayer() == "W")
								? "White Player's Turn"
								: "Black Player's Turn";
							sounds.playCastle();
						}
						else
						{
							statusMsg = "Castling not possible!";
						}
					}
				}
			}
		}

		// ── Check highlight ke liye king position dhundo ─────────

		int kingRow = -1;
		int kingCol = -1;
		bool inCheck = board.isInCheck(board.getCurrentPlayer());

		if (inCheck)
		{
			board.getKingPosition(board.getCurrentPlayer(), kingRow, kingCol);
		}

		// ── Render ───────────────────────────────────────────────

		window.clear(sf::Color(18, 18, 18));
		window.draw(boardSprite);

		drawHighlights(window, selRow, selCol, validMoves, inCheck, kingRow, kingCol);

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if (board.getGrid(i, j))
				{
					drawPiece(window, textures, board.getGrid(i, j)->getSymbol(), i, j);
				}
			}
		}

		drawSidePanel(window, font, board.getCurrentPlayer(),
			statusMsg, board.getMoveCount(), gameOver);

		if (gameOver)
		{
			drawGameOverOverlay(window, font, statusMsg);
		}

		window.display();
	}

	cout << "Chess game closed." << endl;
	return 0;
}