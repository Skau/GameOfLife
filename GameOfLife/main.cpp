#include <SFML/Graphics.hpp>
#include <iostream>

// * THE RULES * \\

//For a space that is 'populated':
//Each cell with one or no neighbors dies, as if by solitude.
//Each cell with four or more neighbors dies, as if by overpopulation.
//Each cell with two or three neighbors survives.

//For a space that is 'empty' or 'unpopulated'
//Each cell with three neighbors becomes populated.

struct Tile;
typedef std::vector<std::vector<Tile*>> GridArray;


static const int ROWS = 100;
static const int COLUMNS = 100;

struct Tile
{
	sf::Vector2f pos;
	bool isPopulated;
	int neighbors;

	Tile(sf::Vector2f inPos) : pos{ inPos }, isPopulated{ false }, neighbors{ 0 }
	{
	}

	int checkNeighbors(GridArray& gridArray)
	{
		neighbors = 0;

		// Right neighbor
		if ((pos.x + 8) / 8 < ROWS && (pos.x + 8) / 8 >= 0 && pos.y / 8 < COLUMNS && pos.y / 8 >= 0)
		{
			if (gridArray[((int)pos.x + 8) / 8][(int)pos.y / 8]->isPopulated)
			{
				neighbors++;
			}
		}

		// Left neighbor
		if ((pos.x - 8) / 8 < ROWS && (pos.x - 8) / 8 >= 0 && pos.y / 8 < COLUMNS && pos.y / 8 >= 0)
		{
			if (gridArray[((int)pos.x - 8) / 8][(int)pos.y / 8]->isPopulated)
			{
				neighbors++;
			}
		}

		// Bottom Neighbor
		if (pos.x / 8 < ROWS && pos.x / 8 >= 0 && (pos.y + 8) / 8 < COLUMNS && (pos.y + 8) / 8 >= 0)
		{
			if (gridArray[(int)pos.x / 8][((int)pos.y + 8) / 8]->isPopulated)
			{
				neighbors++;
			}
		}

		// Top Neighbor
		if (pos.x / 8 < ROWS && pos.x / 8 >= 0 && pos.x / 8 >= 0 && (pos.y - 8) / 8 < COLUMNS && (pos.y - 8) / 8 >= 0)
		{
			if (gridArray[(int)pos.x / 8][((int)pos.y - 8) / 8]->isPopulated)
			{
				neighbors++;
			}
		}

		// Bottom right neighbor
		if ((pos.x + 8) / 8 < ROWS && (pos.x + 8) / 8 >= 0 && (pos.y + 8) / 8 < COLUMNS && (pos.y + 8) / 8 >= 0)
		{
			if (gridArray[(int)(pos.x + 8) / 8][((int)pos.y + 8) / 8]->isPopulated)
			{
				neighbors++;
			}
		}

		// Top right neighbor
		if ((pos.x + 8) / 8 < ROWS && (pos.x + 8) / 8 >= 0 && (pos.y - 8) / 8 < COLUMNS && (pos.y - 8) / 8 >= 0)
		{
			if (gridArray[(int)(pos.x + 8) / 8][((int)pos.y - 8) / 8]->isPopulated)
			{
				neighbors++;
			}
		}

		// Bottom Left neighbor
		if ((pos.x - 8) / 8 < ROWS && (pos.x - 8) / 8 >= 0 && (pos.y + 8) / 8 < COLUMNS && (pos.y + 8) / 8 >= 0)
		{
			if (gridArray[(int)(pos.x - 8) / 8][((int)pos.y + 8) / 8]->isPopulated)
			{
				neighbors++;
			}
		}

		// Top left neighbor
		if ((pos.x - 8) / 8 < ROWS && (pos.x - 8) / 8 >= 0 && (pos.y - 8) / 8 < COLUMNS && (pos.y - 8) / 8 >= 0)
		{
			if (gridArray[(int)(pos.x - 8) / 8][((int)pos.y - 8) / 8]->isPopulated)
			{
				neighbors++;
			}
		}

		return neighbors;
	}
};

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 800), "Conways Game of Life!");

	sf::Image unpopulatedImage;
	unpopulatedImage.loadFromFile("resources/unpopulatedImage.png");
	sf::Texture unpopulatedTexture;
	unpopulatedTexture.loadFromImage(unpopulatedImage);
	sf::Sprite unpopulatedSprite;
	unpopulatedSprite.setTexture(unpopulatedTexture);
	sf::Image populatedImage;
	populatedImage.loadFromFile("resources/populatedImage.png");
	sf::Texture populatedTexture;
	populatedTexture.loadFromImage(populatedImage);
	sf::Sprite populatedSprite;
	populatedSprite.setTexture(populatedTexture);

	std::vector<std::vector<Tile*>> gridArray;
	gridArray.resize(ROWS, std::vector<Tile*>(COLUMNS));

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLUMNS; j++)
		{
			gridArray[i][j] = new Tile(sf::Vector2f(i * 8.0f, j * 8.0f));
		}
	}

	bool isSimulating = false;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
				isSimulating = !isSimulating;
			if (!isSimulating)
			{
				if (event.type == sf::Event::MouseButtonPressed)
				{
					if (event.key.code == sf::Mouse::Button::Left)
					{
						sf::Vector2i pos = sf::Mouse::getPosition(window);
						std::cout << pos.x / 8 << ", " << pos.y / 8 << std::endl;
						gridArray[pos.x / 8][pos.y / 8]->isPopulated = !gridArray[pos.x / 8][pos.y / 8]->isPopulated;
					}
					else if (event.key.code == sf::Mouse::Button::Right)
					{
						sf::Vector2i pos = sf::Mouse::getPosition(window);
						std::cout << gridArray[pos.x / 8][pos.y / 8]->checkNeighbors(gridArray) << std::endl;
					}
				}
			}
		}

		if (isSimulating)
		{
			for (int i = 0; i < ROWS; i++)
			{
				for (int j = 0; j < COLUMNS; j++)
				{
					int neighbors = gridArray[i][j]->checkNeighbors(gridArray);
					if (neighbors <= 1 || neighbors >= 4)
					if (neighbors == 2 || neighbors == 3)
						gridArray[i][j]->isPopulated = true;
				}
			}
		}
	

		window.clear();

		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLUMNS; j++)
			{
				if (gridArray[i][j]->isPopulated)
				{
					populatedSprite.setPosition(gridArray[i][j]->pos);
					window.draw(populatedSprite);
				}
				else
				{
					unpopulatedSprite.setPosition(gridArray[i][j]->pos);
					window.draw(unpopulatedSprite);
				}

			}
		}

		window.display();
	}

	return 0;
}