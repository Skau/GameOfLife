#include <SFML/Graphics.hpp>
#include <iostream>

// * THE RULES * \\

// If an occupied cell has zero or one neighbors, it dies of loneliness.
// If an occupied cell has more than three neighbors, it dies of overcrowding.
// If an empty cell has exactly three occupied neighbor cells, there is a birth of a new cell to replace the empty cell.

struct Tile;


// Number of rows / columns
static const int ROWS = 100;
static const int COLUMNS = 100;

// Forward declaration of the grid
std::vector<std::vector<Tile*>> gridArray;

// Fixed time step stuff
sf::Clock frameClock;
const sf::Time timePerFrame = sf::seconds(1.0f / 20.f);
sf::Time timeSinceLastUpdate;


// What we fill the grid with
struct Tile
{
	sf::Vector2f pos;
	bool alive;
	int neighbors;
	bool nextAlive;
	Tile(sf::Vector2f inPos) : pos{ inPos }, alive{ false }, nextAlive{ false }, neighbors { 0 }
	{
	}
};

// Forward declaration of function
int checkNeighbors(sf::Vector2f pos);

int main()
{
	// Create window and set up the two sprites used
	sf::RenderWindow window(sf::VideoMode(800, 800), "Conways Game of Life!");
	sf::Image deadImage;
	deadImage.loadFromFile("resources/unpopulatedImage.png");
	sf::Texture deadTexture;
	deadTexture.loadFromImage(deadImage);
	sf::Sprite deadSprite;
	deadSprite.setTexture(deadTexture);
	sf::Image aliveImage;
	aliveImage.loadFromFile("resources/populatedImage.png");
	sf::Texture aliveTexture;
	aliveTexture.loadFromImage(aliveImage);
	sf::Sprite aliveSprite;
	aliveSprite.setTexture(aliveTexture);
	sf::Image deadImageBordered;
	deadImageBordered.loadFromFile("resources/unpopulatedImagebordered.png");

	// Prepares the grid
	gridArray.resize(ROWS, std::vector<Tile*>(COLUMNS));

	// Fills the grid (offset of 8 because the image size is 8*8)
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLUMNS; j++)
		{
			gridArray[i][j] = new Tile(sf::Vector2f(i * 8.0f, j * 8.0f));
		}
	}

	// Bool to start the simulation (toggled with spacebar) 
	bool isSimulating = false;

	while (window.isOpen())
	{
		timeSinceLastUpdate += frameClock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;

			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
					isSimulating = !isSimulating;
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R)
				{
					for (int i = 0; i < ROWS; i++)
					{
						for (int j = 0; j < COLUMNS; j++)
						{
							gridArray[i][j]->alive = false;
						}
					}

					if (isSimulating)
						isSimulating = false;
				}
				// Can only add new tiles when not simulating
				if (!isSimulating)
				{
					if (event.type == sf::Event::MouseButtonPressed)
					{
						if (event.key.code == sf::Mouse::Button::Left)
						{
							sf::Vector2i pos = sf::Mouse::getPosition(window);
							std::cout << pos.x << ", " << pos.y << std::endl;
							gridArray[pos.x / 8][pos.y / 8]->alive = !gridArray[pos.x / 8][pos.y / 8]->alive;
						}
					}
				}
			}

			// The simulation
			if (isSimulating)
			{
				// Check neighbors and set next state
				for (int i = 0; i < ROWS; i++)
				{
					for (int j = 0; j < COLUMNS; j++)
					{
						int neighbors = checkNeighbors(gridArray[i][j]->pos);
						if (gridArray[i][j]->alive)
						{
							if (neighbors < 2 || neighbors > 3)
								gridArray[i][j]->nextAlive = false;
							else
								gridArray[i][j]->nextAlive = true;
						}
						else
						{
							if (neighbors == 3)
								gridArray[i][j]->nextAlive = true;
							else
								gridArray[i][j]->nextAlive = false;
						}
					}
				}

				// Set the next state to current state
				for (int i = 0; i < ROWS; i++)
				{
					for (int j = 0; j < COLUMNS; j++)
					{
						gridArray[i][j]->alive = gridArray[i][j]->nextAlive;
					}
				}
			}

			window.clear();

			if (isSimulating)
				deadTexture.loadFromImage(deadImage);
			else
				deadTexture.loadFromImage(deadImageBordered);

			// Render the grid
			for (int i = 0; i < ROWS; i++)
			{
				for (int j = 0; j < COLUMNS; j++)
				{
					if (gridArray[i][j]->alive)
					{
						aliveSprite.setPosition(gridArray[i][j]->pos);
						window.draw(aliveSprite);
					}
					else
					{
						deadSprite.setPosition(gridArray[i][j]->pos);
						window.draw(deadSprite);
					}

				}
			}

			window.display();
		}
	}

	return 0;
}

int checkNeighbors(sf::Vector2f pos)
{
	int neighbors = 0;

	// Right neighbor
	if ((pos.x + 8) / 8 < ROWS && (pos.x + 8) / 8 >= 0 && pos.y / 8 < COLUMNS && pos.y / 8 >= 0)
	{
		if (gridArray[((int)pos.x + 8) / 8][(int)pos.y / 8]->alive)
		{
			neighbors++;
		}
	}

	// Left neighbor
	if ((pos.x - 8) / 8 < ROWS && (pos.x - 8) / 8 >= 0 && pos.y / 8 < COLUMNS && pos.y / 8 >= 0)
	{
		if (gridArray[((int)pos.x - 8) / 8][(int)pos.y / 8]->alive)
		{
			neighbors++;
		}
	}

	// Bottom Neighbor
	if (pos.x / 8 < ROWS && pos.x / 8 >= 0 && (pos.y + 8) / 8 < COLUMNS && (pos.y + 8) / 8 >= 0)
	{
		if (gridArray[(int)pos.x / 8][((int)pos.y + 8) / 8]->alive)
		{
			neighbors++;
		}
	}

	// Top Neighbor
	if (pos.x / 8 < ROWS && pos.x / 8 >= 0 && pos.x / 8 >= 0 && (pos.y - 8) / 8 < COLUMNS && (pos.y - 8) / 8 >= 0)
	{
		if (gridArray[(int)pos.x / 8][((int)pos.y - 8) / 8]->alive)
		{
			neighbors++;
		}
	}

	// Bottom right neighbor
	if ((pos.x + 8) / 8 < ROWS && (pos.x + 8) / 8 >= 0 && (pos.y + 8) / 8 < COLUMNS && (pos.y + 8) / 8 >= 0)
	{
		if (gridArray[(int)(pos.x + 8) / 8][((int)pos.y + 8) / 8]->alive)
		{
			neighbors++;
		}
	}

	// Top right neighbor
	if ((pos.x + 8) / 8 < ROWS && (pos.x + 8) / 8 >= 0 && (pos.y - 8) / 8 < COLUMNS && (pos.y - 8) / 8 >= 0)
	{
		if (gridArray[(int)(pos.x + 8) / 8][((int)pos.y - 8) / 8]->alive)
		{
			neighbors++;
		}
	}

	// Bottom Left neighbor
	if ((pos.x - 8) / 8 < ROWS && (pos.x - 8) / 8 >= 0 && (pos.y + 8) / 8 < COLUMNS && (pos.y + 8) / 8 >= 0)
	{
		if (gridArray[(int)(pos.x - 8) / 8][((int)pos.y + 8) / 8]->alive)
		{
			neighbors++;
		}
	}

	// Top left neighbor
	if ((pos.x - 8) / 8 < ROWS && (pos.x - 8) / 8 >= 0 && (pos.y - 8) / 8 < COLUMNS && (pos.y - 8) / 8 >= 0)
	{
		if (gridArray[(int)(pos.x - 8) / 8][((int)pos.y - 8) / 8]->alive)
		{
			neighbors++;
		}
	}

	gridArray[pos.x / 8][pos.y / 8]->neighbors = neighbors;

	return neighbors;
}