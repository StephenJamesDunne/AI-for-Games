#ifndef FLOWFIELD_HPP
#define FLOWFIELD_HPP

#include <SFML/Graphics.hpp>
#include <vector>

struct Tile
{
    int terrainCost = 1;                    // Terrain traversal cost: 1 = passable, 255 = obstacle
    int cost = -1;                          // (Step 1 Cost Field) Path distance from goal. -1 = unvisited
    float integrationCost = -1.0f;          // (Step 2 Integration Field) Euclidean + cost field. -1 = unvisited
    sf::Vector2i flowDirection = {0, 0};    // (Step 3 Vector field) Direction to lowest integration cost neighbor
};

class FlowField
{
public:
    FlowField(int gridWidth, int gridHeight, float tileSize);

    // Coordinate conversions
    sf::Vector2i worldToGrid(sf::Vector2f worldPos) const;
    sf::Vector2f gridToWorld(int x, int y) const;
    sf::Vector2f getTileCenter(int x, int y) const;

    // Grid and flowfield generation
    void initializeTerrain();
    void createCostField();
    void createIntegrationField();

    // Rendering
    void createFlowArrows(sf::RenderWindow& window, int x, int y, sf::Vector2i direction) const;
    void render(sf::RenderWindow& window);

	// User interactions with the flowfield
    void setStart(sf::Vector2f worldPos);
    void setGoal(sf::Vector2f worldPos);
	void toggleObstacle(sf::Vector2f worldPos);
    bool loadFont(const std::string& fontPath);

    // Display toggles
    void toggleCostField();
    void toggleHeatmap();
    void toggleIntegrationField();
    void toggleVectorField();

    // Visualization of NPC following the flow field
    void findPath(sf::Time deltaTime);
    void resetNPC();
	void calculateShortestPath();
	void drawShortestPath(sf::RenderWindow& window);

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

    int gridWidth;
    int gridHeight;
    float tileSize;

	int maxCostValue{ 0 };          // Maximum cost value for heatmap scaling so that colors are relative to current costs
    bool showHeatmap = false;
    bool showVectorField = false;

    // Made these positions negative so the tile at (0,0) is not start or goal by default
    sf::Vector2i startPosition{ -1, -1 };
    sf::Vector2i goalPosition{ -1, -1 };

    std::vector<std::vector<Tile>> grid;            // Grid of tiles for flowfield positions and costs
    std::vector<sf::RectangleShape> tileShapes;     // Visualization of squares on top of the grid

    // UI elements
    sf::RectangleShape UIBox;
    const float UI_WIDTH = 420.0f;
    sf::Font uiFont;
    sf::Text instructionsText{ uiFont };
    sf::Text costText{ uiFont };

	// Entity following the flow field
	sf::CircleShape npc;
	sf::Vector2f npcPos;
	bool npcActive = false;
	const float NPC_SPEED = 3.0f; // Grid tiles per second

	// Shortest path visualization
	std::vector<sf::Vector2i> shortestPath;
	int currentPathIndex = 0;
	bool showShortestPath = false;

	// Helper functions
    bool isValid(int x, int y) const;
    bool mouseIsInUI(sf::Vector2f mousePos) const;
    bool tileIsObstacle(int x, int y) const;
	bool tileIsReachable(int x, int y) const;
    sf::Vector2i getFlowDirection(int x, int y) const;
	sf::Vector2f normalizeVector(sf::Vector2f vec) const;
	bool isDiagonalBlocked(int fromX, int fromY, int toX, int toY) const;
};

#endif