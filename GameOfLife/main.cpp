#include <SFML/Graphics.hpp>
#include <iostream>
#include "ButtonClass.h"

struct Tile;

// Number of rows / columns
static const int ROWS = 100;
static const int COLUMNS = 100;

// Forward declaration of the grid
std::vector<std::vector<Tile*>> gridArray;

// Fixed time step stuff
sf::Clock frameClock;
sf::Time timePerFrame = sf::seconds(1.0f / 20.f);
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

// Forward declaration of functions
int checkNeighbors(sf::Vector2f pos);
void presetOne();
void presetTwo();
void presetThree();

int main()
{
	// Create window and set up the two sprites used
	sf::RenderWindow app_window(sf::VideoMode(800, 900), "Conways Game of Life!", sf::Style::Titlebar | sf::Style::Close);
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

	std::vector<ButtonClass*> buttons;

	ButtonClass* button_ToggleSimulation = new ButtonClass(sf::Vector2f(140, 850), 250, 50, "Toggle simulation");
	buttons.push_back(button_ToggleSimulation);
	ButtonClass* button_Reset = new ButtonClass(sf::Vector2f(330, 850), 100, 50, "Reset");
	buttons.push_back(button_Reset);
	ButtonClass* button_preset1 = new ButtonClass(sf::Vector2f(420, 875), 35, 30, "1");
	buttons.push_back(button_preset1);
	ButtonClass* button_preset2 = new ButtonClass(sf::Vector2f(470, 875), 35, 30, "2");
	buttons.push_back(button_preset2);
	ButtonClass* button_preset3 = new ButtonClass(sf::Vector2f(520, 875), 35, 30, "3");
	buttons.push_back(button_preset3);
	ButtonClass* button_IncreaseSpeed = new ButtonClass(sf::Vector2f(675, 830), 240, 35, "Increase speed");
	buttons.push_back(button_IncreaseSpeed);
	ButtonClass* button_DecreaseSpeed = new ButtonClass(sf::Vector2f(675, 875), 240, 35, "Decrease speed");
	buttons.push_back(button_DecreaseSpeed);

	button_ToggleSimulation->setOnClick([&]() {
		isSimulating = !isSimulating;
	});

	button_IncreaseSpeed->setOnClick([&]() {
		timePerFrame -= sf::seconds(0.005f);
	});

	button_DecreaseSpeed->setOnClick([&]() {
		timePerFrame += sf::seconds(0.005f);
	});

	button_Reset->setOnClick([&](){
		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLUMNS; j++)
			{
				gridArray[i][j]->alive = false;
			}
		} 
		if (isSimulating)
			isSimulating = false;
	});

	button_preset1->setOnClick(&presetOne);
	button_preset2->setOnClick(&presetTwo);
	button_preset3->setOnClick(&presetThree);

	sf::Font font;
	font.loadFromFile("resources/arial.ttf");
	sf::Text text;
	text.setFont(font);
	text.setString("Premade");
	text.setCharacterSize(32);
	text.setPosition(sf::Vector2f(405, 810));

	while (app_window.isOpen())
	{
		timeSinceLastUpdate += frameClock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;

			sf::Event event;
			while (app_window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					app_window.close();
				if (event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Button::Left)
				{
					for (auto& button : buttons)
					{
						if (button->getIsHovered())
						{
							button->onClick();
						}
					}
				}

				// Can only add new tiles when not simulating
				if (!isSimulating)
				{
					if (event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Button::Left)
					{
						sf::Vector2i pos = sf::Mouse::getPosition(app_window);
						if (pos.x  / 8 < ROWS && pos.x / 8 >= 0 && pos.y / 8 < COLUMNS && pos.y / 8 >= 0)
						gridArray[pos.x / 8][pos.y / 8]->alive = !gridArray[pos.x / 8][pos.y / 8]->alive;
					}
				}
			}

			for (auto& button : buttons)
			{
				button->Tick(sf::Mouse::getPosition(app_window));
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

			app_window.clear();

			for (auto& button : buttons)
			{
				button->render(app_window);
			}

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
						app_window.draw(aliveSprite);
					}
					else
					{
						deadSprite.setPosition(gridArray[i][j]->pos);
						app_window.draw(deadSprite);
					}

				}
			}

			app_window.draw(text);

			app_window.display();
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

	gridArray[(int)pos.x / 8][(int)pos.y / 8]->neighbors = neighbors;

	return neighbors;
}

void presetOne()
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLUMNS; j++)
		{
			if (i == 44 && j == 46)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 45 && j == 45)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 46 && j == 45)
			{ 
				gridArray[i][j]->alive = true;
			}
			if (i == 46 && j == 46)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 48 && j == 46)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 48 && j == 45)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 48 && j == 44)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 47 && j == 44)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 48 && j == 42)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 46 && j == 42)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 45 && j == 42)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 44 && j == 42)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 44 && j == 43)
			{
				gridArray[i][j]->alive = true;
			}
		}
	}
}

void presetTwo()
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLUMNS; j++)
		{
			if (i == 46 && j == 49)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 48 && j == 49)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 48 && j == 48)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 50 && j == 47)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 50 && j == 46)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 50 && j == 45)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 52 && j == 46)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 52 && j == 45)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 52 && j == 44)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 53 && j == 45)
			{
				gridArray[i][j]->alive = true;
			}
		}
	}

}

void presetThree()
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLUMNS; j++)
		{
			if (i == 27 && j == 43)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 27 && j == 44)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 28 && j == 43)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 28 && j == 44)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 37 && j == 43)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 37 && j == 44)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 37 && j == 45)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 38 && j == 42)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 39 && j == 41)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 40 && j == 41)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 38 && j == 46)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 39 && j == 47)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 40 && j == 47)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 40 && j == 47)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 42 && j == 46)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 43 && j == 45)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 43 && j == 44)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 44 && j == 44)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 43 && j == 43)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 42 && j == 42)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 41 && j == 44)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 47 && j == 43)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 48 && j == 43)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 48 && j == 42)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 47 && j == 42)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 47 && j == 41)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 48 && j == 41)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 49 && j == 40)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 49 && j == 44)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 51 && j == 40)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 51 && j == 39)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 51 && j == 44)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 51 && j == 45)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 61 && j == 41)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 62 && j == 41)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 62 && j == 42)
			{
				gridArray[i][j]->alive = true;
			}
			if (i == 61 && j == 42)
			{
				gridArray[i][j]->alive = true;
			}
		}
	}
}
