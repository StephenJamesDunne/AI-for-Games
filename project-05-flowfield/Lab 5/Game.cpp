#include "Game.h"
#include <iostream>

Game::Game() :
	window{ sf::VideoMode{ sf::Vector2u{1050U, 810U}, 32U }, "Flowfield - Cost Field" }
{
	// Create flowfield when game object is created
	flowField = new FlowField(GRID_WIDTH, GRID_HEIGHT, TILE_SIZE);

	if (!flowField->loadFont("ASSETS/FONTS/Jersey20-Regular.ttf"))
	{
		std::cout << "Error loading font." << std::endl;
	}
}

Game::~Game()
{
	delete flowField;
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
		if (newEvent->is<sf::Event::Closed>())
		{
			exitGame = true;
		}
		if (newEvent->is<sf::Event::KeyPressed>())
		{
			processKeys(newEvent);
		}
		if (newEvent->is<sf::Event::MouseButtonPressed>())
		{
			processMouseClick(newEvent);
		}
	}
}

void Game::processKeys(const std::optional<sf::Event> t_event)
{
	const sf::Event::KeyPressed* newKeypress = t_event->getIf<sf::Event::KeyPressed>();
	if (sf::Keyboard::Key::Escape == newKeypress->code)
	{
		exitGame = true;
	}
	else if (sf::Keyboard::Key::Num1 == newKeypress->code)
	{
		flowField->toggleCostField();
	}
}

void Game::processMouseClick(const std::optional<sf::Event> t_event)
{
	const sf::Event::MouseButtonPressed* mousePress = t_event->getIf<sf::Event::MouseButtonPressed>();

	if (mousePress)
	{
		sf::Vector2f mousePos(static_cast<float>(mousePress->position.x),
			static_cast<float>(mousePress->position.y));

		// Left click: Set goal (green)
		if (mousePress->button == sf::Mouse::Button::Left)
		{
			flowField->setGoal(mousePos);
		}
		// Right click: Set start (blue)
		else if (mousePress->button == sf::Mouse::Button::Right)
		{
			flowField->setStart(mousePos);
		}
	}
}

void Game::update(sf::Time t_deltaTime)
{
	if (exitGame)
	{
		window.close();
	}
}

void Game::render()
{
	window.clear(sf::Color::Black);
	flowField->render(window);
	window.display();
}