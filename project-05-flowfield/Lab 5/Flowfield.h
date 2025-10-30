#ifndef FLOWFIELD_HPP
#define FLOWFIELD_HPP

#include <SFML/Graphics.hpp>
#include <vector>

struct Tile
{
    int terrainCost = 1;                    // Terrain traversal cost: 1 = passable, 255 = obstacle
    int cost = -1;                          // Path distance from goal (Step 1 Cost Field), -1 = unvisited
    int integrationCost = -1;               // Euclidean + cost field (Step 2 Integration Field), -1 = unvisited
    sf::Vector2i flowDirection = {0, 0};    // Direction to lowest integration cost neighbor
};

class FlowField
{
public:
    FlowField(int gridWidth, int gridHeight, float tileSize);
    sf::Vector2f worldToGrid(sf::Vector2f worldPos) const;
    void initializeTerrain();
    void createCostField();
    void createIntegrationField();
    void createVectorField(sf::RenderWindow& window, int x, int y, sf::Vector2i direction) const;
    void render(sf::RenderWindow& window);
    void setStart(sf::Vector2f worldPos);
    void setGoal(sf::Vector2f worldPos);
    bool mouseIsInUI(sf::Vector2f mousePos) const;
    bool tileIsObstacle(sf::Vector2f mousePos) const;
    bool loadFont(const std::string& fontPath);
    void toggleCostField();
    void toggleHeatmap();
	void toggleIntegrationField();
    void toggleVectorField();
    sf::Vector2i getFlowDirection(int x, int y) const;

private:
    enum class DisplayMode
    {
        NONE,
        COST_FIELD,
		INTEGRATION_FIELD
    };

	DisplayMode displayMode{ DisplayMode::NONE };

	static constexpr int NEIGHBOUR_COUNT = 8;
	         // Offsets for 8 neighboring tiles (N, S, E, W, NW, NE, SW, SE)
	static constexpr int DX[NEIGHBOUR_COUNT] = { 0, 0, 1, -1, -1, 1, -1, 1 };
	static constexpr int DY[NEIGHBOUR_COUNT] = { -1, 1, 0, 0, -1, -1, 1, 1 };

	static bool isCardinalDirection(int direction) { return direction < 4; }

    int gridWidth;
    int gridHeight;
    float tileSize;

	int maxCostValue{ 0 }; // For heatmap normalization in createCostField()
    bool showHeatmap = false;
    bool showVectorField = false;

    // Made these positions negative so the tile at (0,0) is not start or goal by default
    sf::Vector2f startPosition{ -25.0f, -25.0f };
    sf::Vector2f goalPosition{ -25.0f, -25.0f };

    std::vector<std::vector<Tile>> grid;            // Grid of tiles for flowfield positions and costs
    std::vector<sf::RectangleShape> tileShapes;     // Visualization of squares on top of the grid

    // UI elements
    sf::RectangleShape UIBox;
    const float UI_WIDTH = 210.0f;
    sf::Font uiFont;
    sf::Text instructionsText{ uiFont };
    sf::Text costText{ uiFont };

	// Function to check if coordinates are within grid bounds
    bool isValid(int x, int y) const;
};

#endif