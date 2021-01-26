// RedLynx Gameplay Programmer Technical Assignment
// Version 1.04

#pragma once

#include <SFML/Graphics.hpp>

class Game
{
public:
	Game(const sf::Texture& pawnTexture, const sf::Texture& blockTexture, const sf::Texture& pointTexture, const sf::Font& font);
	~Game() = default;

	void update(const sf::Time elapsedTime);
	void draw(sf::RenderWindow& window);

	void onJumpAction();
	void setMovement(sf::Vector2i direction);

	static sf::String getName()
	{
		return "RedLynx Gameplay Programmer Technical Assignment";
	}

private:

	sf::Sprite m_pawnSprite;
	sf::Sprite m_blockSprite;
	sf::Sprite m_pointSprite;

	sf::Text m_jumpText;
	sf::Text m_moveText;

	float m_jumpTimerSeconds = 0.0f;
};