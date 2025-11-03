#ifndef GAME_HPP
#define GAME_HPP
#pragma warning( push )
#pragma warning( disable : 4275 )

#include <SFML/Graphics.hpp>
#include "Flowfield.h"

class Game
{
public:
	Game();
	~Game();
	void run();

private:
	void processEvents();
	void processKeys(const std::optional<sf::Event> t_event);
	void processMouseClick(const std::optional<sf::Event> t_event);
	void update(sf::Time t_deltaTime);
	void render();

	sf::RenderWindow window;

	// Flowfield instance
	FlowField* flowField;
	const int GRID_WIDTH = 28;
	const int GRID_HEIGHT = 27;
	const float TILE_SIZE = 60.0f;

	bool exitGame = false;
};

#pragma warning( pop ) 
#endif