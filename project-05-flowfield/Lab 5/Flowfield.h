#ifndef FLOWFIELD_HPP
#define FLOWFIELD_HPP

#include <SFML/Graphics.hpp>
#include <vector>

struct Tile
{
    int cost = 1;               // Base terrain cost, 1 = normal passable tile
	int integrationCost = -1;   // Cost from this tile to goal, -1 = unvisited by integration field function
};

class FlowField
{
public:
    FlowField(int gridWidth, int gridHeight, float tileSize);
    sf::Vector2f worldToGrid(sf::Vector2f worldPos) const;
    void createCostField();
	void createIntegrationField();
    // void createFlowfield();
    void render(sf::RenderWindow& window);
	void setStart(sf::Vector2f worldPos);
	void setGoal(sf::Vector2f worldPos);
	bool mouseIsInUI(sf::Vector2f mousePos) const;
	bool tileIsObstacle(sf::Vector2f mousePos) const;
	bool loadFont(const std::string& fontPath);
    void toggleCostField();

private:
    int gridWidth;
    int gridHeight;
    float tileSize;
	bool showCostField = false;

	// Made these positions negative so the tile at (0,0) is not start or goal by default
    sf::Vector2f startPosition{ -25.0f, -25.0f };
    sf::Vector2f goalPosition{ -25.0f, -25.0f };

    std::vector<std::vector<Tile>> grid;    // Grid of tiles for flowfield positions and costs
    std::vector<sf::RectangleShape> tileShapes;    // Visualization of squares on top of the grid

    // UI elements
    sf::RectangleShape UIBox;
	const float UI_WIDTH = 210.0f;
    sf::Font uiFont;
    sf::Text instructionsText{ uiFont };
	sf::Text costText{ uiFont };

    bool isValid(int x, int y) const;
};

#endif