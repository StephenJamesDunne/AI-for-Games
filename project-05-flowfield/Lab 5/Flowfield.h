#ifndef FLOWFIELD_HPP
#define FLOWFIELD_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <cmath>

struct Tile
{
    bool traversable = true;
    int cost = INT_MAX;  // Distance from goal
    sf::Vector2f flowDirection = sf::Vector2f(0.0f, 0.0f);
};

class FlowField
{
public:
    FlowField(int gridWidth, int gridHeight, float tileSize);
    
    void setGoal(sf::Vector2i goalPos);
    void setObstacle(sf::Vector2i pos, bool isObstacle);
    void generateFlowField();
    void render(sf::RenderWindow& window);
    
    sf::Vector2f getFlowDirection(sf::Vector2f worldPos) const;
    bool isTraversable(sf::Vector2i gridPos) const;
    
    void toggleObstacle(sf::Vector2f worldPos);
    sf::Vector2i worldToGrid(sf::Vector2f worldPos) const;

private:
	void calculateCostField();  // Uses Bushfire algorithm for costs from goal
    void calculateFlowField();
    bool isValid(int x, int y) const;
    
    int gridWidth;
    int gridHeight;
    float tileSize;
    
    std::vector<std::vector<Tile>> grid;
    sf::Vector2i goalPosition;
    
	// Visualization of the flow field
    std::vector<sf::RectangleShape> tileShapes;
    std::vector<sf::Vertex> flowLines;
};

#endif