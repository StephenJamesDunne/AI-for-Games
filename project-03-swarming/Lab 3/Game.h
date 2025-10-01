#ifndef GAME_HPP
#define GAME_HPP
#pragma warning( push )
#pragma warning( disable : 4275 )
// ignore this warning
// C:\SFML - 3.0.0\include\SFML\System\Exception.hpp(41, 47) : 
// warning C4275 : non dll - interface class 'std::runtime_error' used as base for dll - interface class 'sf::Exception'


#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include "SteeringBehaviour.h"
#include "Player.h"
#include "NPC.h"
#include <vector>


class Game
{
public:
	Game();
	~Game();
	void run();
	void processEvents();
	void processKeys(const std::optional<sf::Event> t_event);
	void checkKeyboardState();
	void update(sf::Time t_deltaTime);
	void render();

private:

	sf::RenderWindow window; 
	sf::Font font;
	sf::Text enemyBehaviourText{font};
	sf::Text instructionsText{ font };
	bool exitGame = false; 
	//Player* player;
	std::vector<NPC*> npcs;
	LennardJonesSwarm* ljSwarmBehavior;


	std::vector<sf::CircleShape> stars;
	void generateStars();
	void initializeSwarm();
	void cleanupSwarm();

};

#pragma warning( pop ) 
#endif 

