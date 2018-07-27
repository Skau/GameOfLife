#include "ButtonClass.h"
#include <iostream>


ButtonClass::ButtonClass(sf::Vector2f pos, float width, float height, std::string text) : isHovered{ false }
{
	box = std::make_unique<sf::RectangleShape>(sf::Vector2f(width, height));

	box->setFillColor(defaultColor);
	box->setOrigin(sf::Vector2f(width / 2, height / 2));
	box->setPosition(pos);
	box->setOutlineColor(sf::Color::White);
	box->setOutlineThickness(2.f);

	font = std::make_unique<sf::Font>();
	if (!font->loadFromFile("resources/arial.ttf"))
	{
		std::cerr << "Failed to load button font!\n";
	}

	buttonText = std::make_unique<sf::Text>(text, *font, 32);
	sf::FloatRect textRect = buttonText->getLocalBounds();
	buttonText->setOrigin(textRect.left + textRect.width / 2, textRect.top + textRect.height / 2);
	buttonText->setPosition(box->getPosition());
}

void ButtonClass::Tick(sf::Vector2i mousePos)
{
	checkHover(mousePos);

	if (isHovered)
	{
		box->setFillColor(hoveredColor);
	}
	else
	{
		box->setFillColor(defaultColor);
	}
}

void ButtonClass::render(sf::RenderWindow & window)
{
	window.draw(*box);
	window.draw(*buttonText);
}

bool ButtonClass::checkHover(sf::Vector2i mousePos)
{
	if (box->getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y))
	{
		isHovered = true;
	}
	else
	{
		isHovered = false;
	}

	return isHovered;
}

void ButtonClass::onClick()
{
	if (onClickFunc)
	{
		onClickFunc();
	}
}
