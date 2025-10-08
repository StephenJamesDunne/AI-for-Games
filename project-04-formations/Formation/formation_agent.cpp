
#include "stdafx.h"
#include "Agent.h"
#include "Updater.h"
#include "formation.h"
#include <list>

#define PI 3.14159265

const int nagents = 4;
const int npoints = 5;
const int nobstacles = 6;
bool drawAgents = false;
bool drawFormation = true;

int main()
{
	// Create Render Windows
	sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "Steering - Agent");

	// Path creation ***********************************************
	std::vector<Vector2D> tempPath;
	std::vector<Vector2D> wedgePath;

	sf::CircleShape anchor(10); // anchor of the formation
	anchor.setOrigin({ 10, 10 });

	sf::CircleShape slot[nagents];
	for (int i = 0; i < nagents; i++)
	{
		slot[i] = sf::CircleShape(5, 5);
		slot[i].setOrigin({ 5, 5 });
		slot[i].setFillColor(sf::Color::Green);
	}

	float x = 0;
	float y = 0;

	// Create Agents *************************************************
	// Sprite
	sf::Texture spaceship_texture;
	if (!spaceship_texture.loadFromFile("..\\ASSETS\\IMAGES\\Spaceship.png"))
	{
		std::cout << "Error loading Texture";
	}
	spaceship_texture.setSmooth(true);

	sf::Texture background_texture;
	if (!background_texture.loadFromFile("..\\ASSETS\\IMAGES\\background.jpg"))
	{
		std::cout << "Error loading Texture";
	}
	background_texture.setSmooth(true);

	sf::Sprite background{ background_texture };
	background.setTexture(background_texture);
	background.setScale({ 0.8f, 0.8f });

//	sf::Sprite spaceship[nagents];
	// SFML 3.0.0 has a new constructor for sprites that requires a texture !!!!
	sf::Sprite spaceship1{ spaceship_texture };
	sf::Sprite spaceship2{ spaceship_texture };
	sf::Sprite spaceship3{ spaceship_texture };
	sf::Sprite spaceship4{ spaceship_texture };
	sf::Sprite spaceship[4] = {spaceship1, spaceship2, spaceship3, spaceship4};

	// create agents **********************************************
	std::vector<Agent> agent;

	Formation wedge(60);
	wedge.anchor.setpostion(Vector2D(50.f, 50.f));
	wedge.anchor.setState(followpathy, 1, 1);
	// wedge path
	wedgePath.push_back(Vector2D(100.f, 100.f));
	wedgePath.push_back(Vector2D(150.f, 120.f));
	wedgePath.push_back(Vector2D(200.f, 150.f));
	wedgePath.push_back(Vector2D(300.f, 200.f));
	wedgePath.push_back(Vector2D(250.f, 400.f));
	wedgePath.push_back(Vector2D(300.f, 500.f));
	wedgePath.push_back(Vector2D(500.f, 450.f));
	wedgePath.push_back(Vector2D(600.f, 400.f));
	wedgePath.push_back(Vector2D(800.f, 500.f));
	wedgePath.push_back(Vector2D(900.f, 650.f));
	wedgePath.push_back(Vector2D(1000.f, 600.f));

	wedge.anchor.setPath(wedgePath);

	//Drawable wedge path 
	sf::VertexArray linesWedge(sf::PrimitiveType::LineStrip, wedgePath.size());
	int i = 0;
	//	for each (Vector2D index in wedgePath)
	for (Vector2D index : wedgePath)
	{
		linesWedge[i].position = sf::Vector2f(index.x, index.y);
		linesWedge[i].color = sf::Color(153, 204, 102);
		i++;
	}

	for (int i = 0; i < nagents; i++)
	{
		agent.push_back(Agent(50, 20, 5, 2));

		spaceship[i].setTexture(spaceship_texture);
		spaceship[i].setScale(sf::Vector2f(0.08f, 0.08f)); // absolute scale factor
		spaceship[i].setOrigin(sf::Vector2f(spaceship_texture.getSize().x / 2, spaceship_texture.getSize().y / 2));

		agent[i].setangularvelocity(0.f);
		agent[i].setPath(tempPath);
		agent[i].setpostion(wedge.GetTargetSlot(i).position);
		agent[i].setGroup(obstacley, 3, 3);
		agent[i].setGroup(collisionfovy, 3, 2);
		agent[i].setGroup(separationy, 2, 2);
		agent[i].setGroup(arrivey, 1, 1);
	}

	Updater kinematic;

	// target Radius
	sf::CircleShape TargetRadius(120);
	TargetRadius.setOrigin({ 120, 120 });
	TargetRadius.setFillColor(sf::Color::Transparent);
	TargetRadius.setOutlineThickness(2);

	// collision Radius
	sf::CircleShape collision(10, 10);
	collision.setOrigin({ 10, 10 });
	collision.setFillColor(sf::Color::Blue);

	// texture for obstacles
	sf::Texture obstacle_texture;
	if (!obstacle_texture.loadFromFile("..\\ASSETS\\IMAGES\\Meteor.png"))
	{
		std::cout << "Error loading Texture";
	}
	obstacle_texture.setSmooth(true);

	float trackupdate = 0.f;
	std::list<Vector2D> trackpos;

	// obstacles *************************************************************
	Vector2D position;
	Vector2D dimension;

	std::vector<Obstacle> obstacle;

	std::vector<sf::RectangleShape> rectangle;

	// draw nobstacles obstacles
	// 1.
	position = Vector2D(100, 400);
	dimension = Vector2D(100, 100);
	obstacle.push_back(Obstacle(dimension, position));

	sf::RectangleShape rect(sf::Vector2f(dimension.x, dimension.y));
	rect.setOrigin({ dimension.x / 2, dimension.y / 2 });
	rect.setPosition({ position.x, position.y });
	rect.scale({ 0.8, 0.8 }); // draw rectangle a little bit smaller (collision)
	rect.setTexture(&obstacle_texture);
	rectangle.push_back(rect);

	// 2.
	position = Vector2D(400, 200);
	dimension = Vector2D(100, 100);
	obstacle.push_back(Obstacle(dimension, position));

	rect.setSize(sf::Vector2f(dimension.x, dimension.y));
	rect.setOrigin({ dimension.x / 2, dimension.y / 2 });
	rect.setPosition({ position.x, position.y });
	rectangle.push_back(rect);

	// 3.
	position = Vector2D(450, 300);
	dimension = Vector2D(100, 100);
	obstacle.push_back(Obstacle(dimension, position));

	rect.setSize(sf::Vector2f(dimension.x, dimension.y));
	rect.setOrigin({ dimension.x / 2, dimension.y / 2 });
	rect.setPosition({ position.x, position.y });
	rectangle.push_back(rect);

	// 4.
	position = Vector2D(600, 550);
	dimension = Vector2D(150, 150);
	obstacle.push_back(Obstacle(dimension, position));

	rect.setSize(sf::Vector2f(dimension.x, dimension.y));
	rect.setOrigin({dimension.x / 2, dimension.y / 2});
	rect.setPosition({ position.x, position.y });
	rectangle.push_back(rect);

	// 5.
	position = Vector2D(900, 200);
	dimension = Vector2D(100, 100);
	obstacle.push_back(Obstacle(dimension, position));

	rect.setSize(sf::Vector2f(dimension.x, dimension.y));
	rect.setOrigin({ dimension.x / 2, dimension.y / 2 });
	rect.setPosition({ position.x, position.y });
	rectangle.push_back(rect);

	// 6.
	position = Vector2D(1100, 500);
	dimension = Vector2D(200, 200);
	obstacle.push_back(Obstacle(dimension, position));

	rect.setSize(sf::Vector2f(dimension.x, dimension.y));
	rect.setOrigin({ dimension.x / 2, dimension.y / 2 });
	rect.setPosition({ position.x, position.y });
	rectangle.push_back(rect);

	sf::Font font;
	if (!font.openFromFile("..\\ASSETS\\FONTS\\ariblk.ttf"))
	{
		// error...
	}

	// draw track
	sf::CircleShape track(2);
	track.setFillColor(sf::Color::White);

	// MousePosition vector
	sf::Vector2i MousePosition;

	sf::Clock clock;
	sf::Time dt;

	// run the program as long as the window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
//		sf::Event event;
		while (const std::optional newEvent = window.pollEvent())
//		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (newEvent->is<sf::Event::Closed>())
//			if (event.type == sf::Event::Closed)
				window.close();
			// set target
			else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				// get global mouse position
				MousePosition = sf::Mouse::getPosition(window);
//				target.setPosition(MousePosition.x, MousePosition.y);

				// ****** Target *****************************
//				targetStruct.position = Vector2D(MousePosition.x, MousePosition.y);
				//std::cout << position.x << " " << position.y << std::endl;
			}
			if (newEvent->is<sf::Event::KeyPressed>())
//			else if (event.type == sf::Event::KeyPressed)
			{
				const sf::Event::KeyPressed* newKeypress = newEvent->getIf<sf::Event::KeyPressed>();
				if (sf::Keyboard::Key::Escape == newKeypress->code)
				{
					window.close();
				}
				else if (sf::Keyboard::Key::D == newKeypress->code)
				//else if (event.key.code == sf::Keyboard::D)
				{
					drawFormation = !drawFormation;
				}
				else if (sf::Keyboard::Key::A == newKeypress->code)
				//else if (event.key.code == sf::Keyboard::A)
				{
					drawAgents = !drawAgents;
				}
			}
		}
		// clear the window with black color
		window.clear(sf::Color::Black);

		window.draw(background);

		//for each (sf::RectangleShape rect in rectangle)
		for (sf::RectangleShape rect : rectangle)
			{
			window.draw(rect);
		}

		// update trackpos every 0.5sec
		if (trackupdate > .8f)
		{
			trackupdate = 0.f;
			for (int i = 0; i < nagents; i++)
			{
				trackpos.push_back(agent[i].getposition());
			}
		}
		// Footprints
		// draw track positions in list every frame
		// for each (Vector2D vec in trackpos)
		if (drawAgents) {
			for (Vector2D vec : trackpos)
			{
				track.setPosition(sf::Vector2f(vec.x, (vec.y)));
				window.draw(track);
			}
		}

		anchor.setPosition(sf::Vector2f(wedge.anchor.getposition().x, wedge.anchor.getposition().y));

		// draw everything here...
		if (drawFormation) {
			window.draw(anchor);
		}
		window.draw(linesWedge);

		// Show the path nodes
		//sf::CircleShape pp(5);
		//for (Vector2D index : wedgePath)
		//{
		//	pp.setPosition(sf::Vector2f(index.x, index.y));
		//	window.draw(pp);
		//}
		
		//update kinematic with target pos = mousePos ************************************************
		for (int i = 0; i < nagents; i++)
		{
			// group dynamic *****************************************************************
			kinematic.update(wedge.anchor, wedge.getTarget(), agent, obstacle, dt.asSeconds());
			// agents ************************************************************************
			kinematic.update(agent[i], wedge.GetTargetSlot(i), agent, obstacle, dt.asSeconds());

			// check if out of windows size
			agent[i].setpostion(Vector2D(fmodf(agent[i].getposition().x, window.getSize().x), fmodf(agent[i].getposition().y, window.getSize().y)));
			// update position of triangle = agent
			spaceship[i].setPosition({ agent[i].getposition().x, agent[i].getposition().y });
			spaceship[i].setRotation(sf::degrees(agent[i].getorientation().angle * 180 / PI));
			spaceship[i].rotate(sf::degrees(- 180));  // because of Spaceship Sprite is flipped...or start orientation is 0 -> does not match with Sprite...
			if (drawAgents) {
				window.draw(spaceship[i]);
			}
			
			//std::cout << wedge.getSlotPosition(i).x << " " << wedge.getSlotPosition(i).y << std::endl;
			slot[i].setPosition({ wedge.GetTargetSlot(i).position.x, wedge.GetTargetSlot(i).position.y });
			if (drawFormation) {
				window.draw(slot[i]);
			}

			// ******* Draw Rays ********************************************************
//			Vector2D vel = Vector2D(targetStruct.position) - agent[i].getposition();

			// agent velocity (= orientation)
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(agent[i].getposition().x, agent[i].getposition().y),sf::Color::Cyan),
				sf::Vertex(sf::Vector2f(agent[i].getposition().x + agent[i].getvelocity().x * 3.5f,agent[i].getposition().y + agent[i].getvelocity().y * 3.5f),sf::Color::Cyan)
			};
			if (drawAgents) {
				window.draw(line, 2, sf::PrimitiveType::Lines);
			}
		}

		// wedge velocity (= orientation)
		sf::Vertex line[] =
		{
			sf::Vertex(sf::Vector2f(wedge.anchor.getposition().x, wedge.anchor.getposition().y),sf::Color::Cyan),
			sf::Vertex(sf::Vector2f(wedge.anchor.getposition().x + wedge.anchor.getvelocity().x * 3.5f,wedge.anchor.getposition().y + wedge.anchor.getvelocity().y * 3.5f),sf::Color::Cyan)
		};
		if (drawFormation) {
			window.draw(line, 2, sf::PrimitiveType::Lines);
		}

		// end the current frame
		window.display();

		trackupdate += dt.asSeconds();

		dt = clock.restart();
	}

	return 0;

}

