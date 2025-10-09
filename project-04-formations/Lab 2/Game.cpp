#include "Game.h"
#include "SteeringBehaviour.h"
#include <iostream>
#include <filesystem>
#include <cstdlib>


Game::Game()
{
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::Vector2u windowSize = { 1920, 1080 };
	window.create(sf::VideoMode(windowSize), "Lab 4");

	if (!font.openFromFile("ASSETS/FONTS/Jersey20-Regular.ttf"))
	{
		std::cout << "Font failed to load!" << std::endl;
	}

	instructionsText.setString("Formation Showcase - NPC ships will form finger-4 formation with Player as the leader.\nUse WASD to control the player.\nPress ESC to exit");
	instructionsText.setFont(font);
	instructionsText.setCharacterSize(32);
	instructionsText.setFillColor(sf::Color::White);
	instructionsText.setOutlineColor(sf::Color::Black);
	instructionsText.setOutlineThickness(1.0f);
	instructionsText.setPosition(sf::Vector2f(10.0f, 10.0f));

	enemyBehaviourText.setFont(font);
	enemyBehaviourText.setCharacterSize(24);
	enemyBehaviourText.setFillColor(sf::Color::White);
	enemyBehaviourText.setOutlineColor(sf::Color::Black);
	enemyBehaviourText.setOutlineThickness(1.0f);

	generateStars();
	
	// instantiate player and enemy
	player = new Player("ASSETS/IMAGES/player.png");

	// Create three NPCs using Formation behavior with player as leader
	enemies.push_back(std::make_unique<Enemy>("ASSETS/IMAGES/enemy.png", 
    	std::make_unique<Formation>(player, Formation::Position::LEFT_WING), 300.0f));

	enemies.push_back(std::make_unique<Enemy>("ASSETS/IMAGES/enemy2.png", 
    	std::make_unique<Formation>(player, Formation::Position::RIGHT_WING), 350.0f));

	enemies.push_back(std::make_unique<Enemy>("ASSETS/IMAGES/enemy3.png", 
    	std::make_unique<Formation>(player, Formation::Position::TAIL, 
        	Formation::DEFAULT_MAX_ACCELERATION, Formation::DEFAULT_MAX_SPEED,
        	Formation::DEFAULT_SLOWING_RADIUS, Formation::DEFAULT_TARGET_RADIUS,
        	Formation::DEFAULT_TAIL_OFFSET), 400.0f));

    // all enemies start active
    enemyActiveStates.resize(enemies.size(), true);
}

Game::~Game()
{
	delete player;
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

	player->handleInput(t_deltaTime);

	sf::Vector2u windowSize = window.getSize();
	player->update(windowSize, t_deltaTime);

	for (auto& enemy : enemies)
	{
		enemy->update(windowSize, t_deltaTime);

		if (enemy->canSeePlayer(player->getSprite()))
		{
			enemy->setVisionConeColor(sf::Color(255, 0, 0, 80));
		}
		else
		{
			enemy->setVisionConeColor(sf::Color(255, 255, 0, 80));
		}
	}

	if (exitGame)
	{
		window.close();
	}
}


void Game::render()
{
	window.clear(sf::Color::Black);

	for (const auto& star : stars)
	{
		window.draw(star);
	}

	player->draw(window);

	for (size_t i = 0; i < enemies.size(); ++i)
    {
        enemies[i]->draw(window);
        
        enemyBehaviourText.setString(enemies[i]->getBehaviourName());
        enemyBehaviourText.setPosition(enemies[i]->getPosition() + sf::Vector2f(-20.0f, -40.0f));
        window.draw(enemyBehaviourText);
    }

	window.draw(instructionsText);
	
	window.display();
}



