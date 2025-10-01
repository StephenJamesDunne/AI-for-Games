#include "Game.h"
#include <iostream>
#include <filesystem>
#include <cstdlib>


Game::Game()
{
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::Vector2u windowSize = { 1920, 1080 };
	window.create(sf::VideoMode(windowSize), "Lab 3");

	if (!font.openFromFile("ASSETS/FONTS/Jersey20-Regular.ttf"))
	{
		std::cout << "Font failed to load!" << std::endl;
	}

	instructionsText.setFont(font);
	instructionsText.setCharacterSize(32);
	instructionsText.setFillColor(sf::Color::White);
	instructionsText.setOutlineColor(sf::Color::Black);
	instructionsText.setOutlineThickness(1.0f);
	instructionsText.setPosition(sf::Vector2f(10.0f, 10.0f));

	//generateStars();

	initializeSwarm();
}

Game::~Game()
{
	//delete player;
	cleanupSwarm();
}

void Game::generateStars()
{
	sf::Vector2u windowSize = window.getSize();
	const int numStars = 500;

	for (int i = 0; i < numStars; ++i)
	{
		sf::CircleShape star;
		
		float x = static_cast<float>(std::rand() % windowSize.x);
		float y = static_cast<float>(std::rand() % windowSize.y);

		float radius = 0.5f;

		star.setRadius(radius);
		star.setPosition(sf::Vector2f(x, y));
		star.setFillColor(sf::Color::White);
		stars.push_back(star);
	}
}

void Game::initializeSwarm()
{
	ljSwarmBehavior = new LennardJonesSwarm(
		&npcs, 
		5000.0f, 
		10000.0f, 
		1.0f, 
		2.0f, 
		500.0f
	);

	int swarmSize = 100;
	sf::Vector2u windowSize = window.getSize();

	float spawnWidth = 200.0f;
	float spawnHeight = 200.0f;
	float startX = (windowSize.x - spawnWidth) / 2.0f;
	float startY = (windowSize.y - spawnHeight) / 2.0f;

	for (int i = 0; i < swarmSize; ++i) {
        float x = startX + static_cast<float>(rand()) / RAND_MAX * spawnWidth;
        float y = startY + static_cast<float>(rand()) / RAND_MAX * spawnHeight;
        
		NPC* npc = new NPC("ASSETS/IMAGES/bee.png");
		npc->setPosition(sf::Vector2f(x, y));
		npc->setSteeringBehavior(ljSwarmBehavior);
		npcs.push_back(npc);
    } 

	instructionsText.setString("Swarming Demonstration\nNumber of bees in swarm: " + std::to_string(npcs.size()));
}

void Game::cleanupSwarm()
{
	// Clear the vector so that all NPCs are deleted and memory is freed
	for (NPC* npc : npcs) {
		delete npc;
	}
	npcs.clear();

	delete ljSwarmBehavior;
	ljSwarmBehavior = nullptr;
}

void Game::run()
{	
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	const float fps{ 60.0f };
	sf::Time timePerFrame = sf::seconds(1.0f / fps); 
	while (window.isOpen())
	{
		processEvents(); 
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); 
			update(timePerFrame); 
		}
		render(); 
	}
}

void Game::processEvents()
{
	
	while (const std::optional newEvent = window.pollEvent())
	{
		if ( newEvent->is<sf::Event::Closed>()) 
		{
			exitGame = true;
		}
		if (newEvent->is<sf::Event::KeyPressed>()) 
		{
			processKeys(newEvent);
		}
	}
}


void Game::processKeys(const std::optional<sf::Event> t_event)
{
	if (!t_event.has_value())
		return;

	const sf::Event::KeyPressed *newKeypress = t_event->getIf<sf::Event::KeyPressed>();
	if (!newKeypress)
		return;

	if (sf::Keyboard::Key::Escape == newKeypress->code)
	{
		exitGame = true;
	}
}


void Game::checkKeyboardState()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
	{
		exitGame = true;
	}
}

void Game::update(sf::Time t_deltaTime)
{
	checkKeyboardState();

	for (NPC* npc : npcs) {
		npc->update(window.getSize(), t_deltaTime);
	}

	if (exitGame)
	{
		window.close();
	}


}


void Game::render()
{
	window.clear(sf::Color::Black);

	window.draw(instructionsText);

	for (const NPC* npc : npcs) {
		window.draw(npc->getSprite());
	}
	
	window.display();
}



