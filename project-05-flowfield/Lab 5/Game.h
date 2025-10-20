#ifndef GAME_HPP
#define GAME_HPP
#pragma warning( push )
#pragma warning( disable : 4275 )

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
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
	void checkKeyboardState();
	void update(sf::Time t_deltaTime);
	void render();
	void setupSprites();

	// Flowfield related functions
	void processMouseClick(const std::optional<sf::Event> t_event);
	void createRandomObstacles();

	sf::RenderWindow window; 
	sf::Font font;	
	sf::Texture texture;
	sf::Sprite sprite{ texture }; 
	
	// Flow Field
	FlowField* flowField;
	const int GRID_WIDTH = 89;
	const int GRID_HEIGHT = 50;
	const float TILE_SIZE = 21.6f; // 1080p screen / 50 grid size = 14.4
	
	bool exitGame = false; 
};

#pragma warning( pop ) 
#endif