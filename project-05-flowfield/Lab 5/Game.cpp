#include "Game.h"
#include <iostream>

Game::Game() :
	window{ sf::VideoMode{ sf::Vector2u{1920U, 1080U}, 32U }, "Lab 5 - Flowfield Pathfinding" }
{ 
	flowField = new FlowField(GRID_WIDTH, GRID_WIDTH, TILE_SIZE);

	flowField->setGoal(sf::Vector2i(GRID_WIDTH / 2, GRID_HEIGHT / 2));
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

	if (sf::Keyboard::Key::R == newKeypress->code)
	{
		createRandomObstacles();
	}
}

void Game::processMouseClick(const std::optional<sf::Event> t_event)
{
	const sf::Event::MouseButtonPressed* mousePress = t_event->getIf<sf::Event::MouseButtonPressed>();

	if (mousePress)
	{
		sf::Vector2f mousePos(static_cast<float>(mousePress->position.x),
			static_cast<float>(mousePress->position.y));

		// Left click: Set goal
		if (mousePress->button == sf::Mouse::Button::Left)
		{
			sf::Vector2i gridPos = flowField->worldToGrid(mousePos);
			flowField->setGoal(gridPos);
		}
		// Right click: Toggle obstacle
		else if (mousePress->button == sf::Mouse::Button::Right)
		{
			flowField->toggleObstacle(mousePos);
		}
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

	flowField->render(window);
	
	window.display();
}

void Game::setupSprites()
{
	
}

void Game::createRandomObstacles()
{
	// Create some random obstacles (about 15% of the grid)
	for (int i = 0; i < (GRID_HEIGHT * GRID_WIDTH) / 7; ++i)
	{
		int x = rand() % GRID_HEIGHT;
		int y = rand() % GRID_WIDTH;
		flowField->setObstacle(sf::Vector2i(x, y), true);
	}
	flowField->generateFlowField();
}
