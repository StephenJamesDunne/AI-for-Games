#include "Game.h"
#include "SteeringBehaviour.h"
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

	instructionsText.setString("Swarming Demonstration");
	instructionsText.setFont(font);
	instructionsText.setCharacterSize(32);
	instructionsText.setFillColor(sf::Color::White);
	instructionsText.setOutlineColor(sf::Color::Black);
	instructionsText.setOutlineThickness(1.0f);
	instructionsText.setPosition(sf::Vector2f(10.0f, 10.0f));

	//generateStars();
}

Game::~Game()
{
	//delete player;
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

	if (exitGame)
	{
		window.close();
	}
}


void Game::render()
{
	window.clear(sf::Color::Black);

	window.draw(instructionsText);
	
	window.display();
}



