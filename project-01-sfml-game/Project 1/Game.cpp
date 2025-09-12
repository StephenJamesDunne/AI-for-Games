#include "Game.h"
#include <iostream>
#include <filesystem>


Game::Game() : player("ASSETS\\IMAGES\\player.png"), enemy("ASSETS\\IMAGES\\enemy.png")
{
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::Vector2u windowSize = { desktop.size.x / 2, desktop.size.y / 2 };
	window.create(sf::VideoMode(windowSize), "SFML Game");
}

Game::~Game()
{
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
	const sf::Event::KeyPressed *newKeypress = t_event->getIf<sf::Event::KeyPressed>();
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

	player.handleInput(t_deltaTime);
	enemy.handleInput(t_deltaTime);

	sf::Vector2u windowSize = window.getSize();
	player.update(windowSize);
	enemy.update(windowSize);

	if (exitGame)
	{
		window.close();
	}
}


void Game::render()
{
	window.clear(sf::Color::Black);

	player.draw(window);
	enemy.draw(window);
	
	window.display();
}



