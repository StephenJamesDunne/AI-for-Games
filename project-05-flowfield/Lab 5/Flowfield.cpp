#include "FlowField.h"
#include <iostream>
#include <string>
#include <queue>
#include <cmath>

FlowField::FlowField(int w, int h, float size)
    : gridWidth(w), gridHeight(h), tileSize(size)
{
    // Resize grid to specified width and height
    grid.resize(gridHeight, std::vector<Tile>(gridWidth));

    // Reserve to help with performance since grid size is constant
    tileShapes.reserve(gridWidth * gridHeight);
    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            sf::RectangleShape rect(sf::Vector2f(tileSize - 2.0f, tileSize - 2.0f));
            rect.setPosition({x * tileSize, y * tileSize});
            rect.setFillColor(sf::Color(70, 70, 70));
            rect.setOutlineColor(sf::Color(40, 40, 40));
            rect.setOutlineThickness(1.0f);
            tileShapes.push_back(rect);
        }
    }

    UIBox.setSize(sf::Vector2f(UI_WIDTH, gridHeight * tileSize));
    UIBox.setPosition(sf::Vector2f(0.0f, 0.0f));
    UIBox.setFillColor(sf::Color(40, 40, 50, 255));
    UIBox.setOutlineColor(sf::Color(100, 100, 120));
    UIBox.setOutlineThickness(2.0f);

    instructionsText.setCharacterSize(24);
    instructionsText.setFillColor(sf::Color(220, 220, 220));
    instructionsText.setOutlineColor(sf::Color::Black);
    instructionsText.setOutlineThickness(2.0f);
    instructionsText.setPosition({10.0f, 10.0f});
    instructionsText.setString(
        "Flowfield Pathfinding\n\nInstructions:\n\n\nSet goal node\nwith left click\n\nSet start node\nwith right click\n\nToggle cost field\nwith '1'\n\nToggle heatmap\nwith '2'\n\nToggle integration\nwith '3'\n\nToggle vector field\nwith '4'");

    costText.setCharacterSize(14);
    costText.setFillColor(sf::Color::White);
    costText.setOutlineColor(sf::Color::Black);
    costText.setOutlineThickness(1.0f);

    initializeTerrain();
}

// Initialize terrain costs (passable tiles and obstacles)
void FlowField::initializeTerrain()
{
    // Initialize all tiles with default terrain cost
    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            grid[y][x].terrainCost = 1; // Normal passable tile
        }
    }

    // Create a vertical wall
    for (int y = 10; y < 20; y++)
    {
        grid[y][25].terrainCost = 255;
    }

    // Create a horizontal wall
    for (int x = 10; x < 20; x++)
    {
        grid[25][x].terrainCost = 255;
    }
}

// Step 1: Create Cost Field - Calculate path distance from goal to every tile
void FlowField::createCostField()
{
    // Reset all path distances
    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            grid[y][x].cost = -1; // -1 = unvisited
        }
    }

    if (goalPosition.x < 0 || goalPosition.y < 0 ||
        !isValid(static_cast<int>(goalPosition.x), static_cast<int>(goalPosition.y)))
    {
        return;
    }

    int goalX = static_cast<int>(goalPosition.x);
    int goalY = static_cast<int>(goalPosition.y);

    // Check if goal is on an obstacle
    if (grid[goalY][goalX].terrainCost == 255)
    {
        return;
    }

    // Mark goal with path distance 0
    grid[goalY][goalX].cost = 0;

    std::queue<sf::Vector2f> validTiles;
    validTiles.push(sf::Vector2f(goalX, goalY));

    // All 8 neighbor directions
    const int dx[] = {0, 0, -1, 1, -1, 1, -1, 1};
    const int dy[] = {-1, 1, 0, 0, -1, -1, 1, 1};

    while (!validTiles.empty())
    {
        sf::Vector2f current = validTiles.front();
        validTiles.pop();

        int currentX = static_cast<int>(current.x);
        int currentY = static_cast<int>(current.y);
        int currentCost = grid[currentY][currentX].cost;

        for (int i = 0; i < NEIGHBOUR_COUNT; i++)
        {
            int neighbourX = currentX + DX[i];
            int neighbourY = currentY + DY[i];

            if (!isValid(neighbourX, neighbourY))
                continue;

            if (grid[neighbourY][neighbourX].terrainCost == 255)
                continue;

            if (grid[neighbourY][neighbourX].cost == -1)
            {
                grid[neighbourY][neighbourX].cost = currentCost + 1;

                if (grid[neighbourY][neighbourX].cost > maxCostValue)
                {
                    maxCostValue = grid[neighbourY][neighbourX].cost;
                }

                validTiles.push(sf::Vector2f(neighbourX, neighbourY));
            }
        }
    }
}

// Step 2: Calculate Integration Field - Euclidean distance + cost field value
void FlowField::createIntegrationField()
{
    // Cost field needs to be created first
    if (goalPosition.x < 0 || goalPosition.y < 0)
        return;

    int goalX = static_cast<int>(goalPosition.x);
    int goalY = static_cast<int>(goalPosition.y);

    // For each tile, integration = cost field + Euclidean distance to goal
    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            // If tile is unreachable (cost == -1) or is an obstacle
            if (grid[y][x].cost == -1 || grid[y][x].terrainCost == 255)
            {
                grid[y][x].integrationCost = -1;
                continue;
            }

            // Calculate Euclidean distance from this tile to goal
            float dx = static_cast<float>(x - goalX);
            float dy = static_cast<float>(y - goalY);
            float euclideanDist = std::sqrt(dx * dx + dy * dy);

            // Integration = cost field + Euclidean distance
            grid[y][x].integrationCost = grid[y][x].cost * tileSize + static_cast<int>(euclideanDist);
        }
    }

	// Get the flow direction for each tile after all integration costs are calculated
    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            grid[y][x].flowDirection = getFlowDirection(x, y);
        }
	}
}

// Step 3: Create visualization of Flow Field using integration costs and flow directions
void FlowField::createVectorField(sf::RenderWindow &window, int x, int y, sf::Vector2i direction) const
{
    if (direction.x == 0 && direction.y == 0)
        return;

	// Staring point = center of tile
    sf::Vector2f start(x * tileSize + tileSize / 2.0f, y * tileSize + tileSize / 2.0f);

	// End point = direction to the goal scaled down to fit in tile
    sf::Vector2f end = start + sf::Vector2f(direction.x, direction.y) * (tileSize / 2.5f);

    sf::VertexArray arrow(sf::PrimitiveType::Lines, 2);
    arrow[0].position = start;
    arrow[0].color = sf::Color::White;
    arrow[1].position = end;
    arrow[1].color = sf::Color::White;

    window.draw(arrow);

    // Calculate arrowhead
    sf::Vector2f arrowDir = end - start;
    float length = std::sqrt(arrowDir.x * arrowDir.x + arrowDir.y * arrowDir.y);

    if (length > 0)
    {
        // Normalize direction
        arrowDir.x /= length;
        arrowDir.y /= length;

        // Perpendicular vector for arrowhead wings
        sf::Vector2f perpendicular(-arrowDir.y, arrowDir.x);

        // Arrowhead size
        float headLength = tileSize / 6.0f;
        float headWidth = tileSize / 8.0f;

        // Calculate two points for arrowhead wings
        sf::Vector2f headPoint1 = end - arrowDir * headLength + perpendicular * headWidth;
        sf::Vector2f headPoint2 = end - arrowDir * headLength - perpendicular * headWidth;

        // Draw arrowhead lines
        sf::VertexArray head1(sf::PrimitiveType::Lines, 2);
        head1[0].position = end;
        head1[0].color = sf::Color::White;
        head1[1].position = headPoint1;
        head1[1].color = sf::Color::White;
        window.draw(head1);

        sf::VertexArray head2(sf::PrimitiveType::Lines, 2);
        head2[0].position = end;
        head2[0].color = sf::Color::White;
        head2[1].position = headPoint2;
        head2[1].color = sf::Color::White;
        window.draw(head2);
    }
}

// Left click for setting the goal position
void FlowField::setGoal(sf::Vector2f worldPos)
{
    if (mouseIsInUI(worldPos) || tileIsObstacle(worldPos))
    {
        return;
    }

    sf::Vector2f gridPos = worldToGrid(worldPos);

    if (isValid(static_cast<int>(gridPos.x), static_cast<int>(gridPos.y)))
    {
        if (gridPos.x != startPosition.x || gridPos.y != startPosition.y)
        {
            goalPosition = gridPos;
            createCostField();
            createIntegrationField();
        }
    }
}

// Right click for setting the start position
void FlowField::setStart(sf::Vector2f worldPos)
{
    if (mouseIsInUI(worldPos) || tileIsObstacle(worldPos))
    {
        return;
    }

    sf::Vector2f gridPos = worldToGrid(worldPos);

    if (isValid(static_cast<int>(gridPos.x), static_cast<int>(gridPos.y)))
    {
        if (gridPos.x != goalPosition.x || gridPos.y != goalPosition.y)
        {
            startPosition = gridPos;
        }
    }
}

bool FlowField::mouseIsInUI(sf::Vector2f mousePos) const
{
    return mousePos.x <= UI_WIDTH;
}

bool FlowField::tileIsObstacle(sf::Vector2f mousePos) const
{
    sf::Vector2f gridPos = worldToGrid(mousePos);
    if (isValid(static_cast<int>(gridPos.x), static_cast<int>(gridPos.y)))
    {
        return grid[gridPos.y][gridPos.x].terrainCost == 255;
    }
    return false;
}

void FlowField::render(sf::RenderWindow &window)
{
    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            int index = y * gridWidth + x;

            // Draw tiles based on their state
            if (x == goalPosition.x && y == goalPosition.y)
            {
                tileShapes[index].setFillColor(sf::Color(50, 200, 50)); // Green for goal node
            }
            else if (x == startPosition.x && y == startPosition.y)
            {
                tileShapes[index].setFillColor(sf::Color(50, 50, 200)); // Blue for start node
            }
            else if (grid[y][x].terrainCost == 255)
            {
                tileShapes[index].setFillColor(sf::Color(255, 0, 0)); // Red for obstacles
            }
            else if (showHeatmap && grid[y][x].cost != -1 && maxCostValue > 0)
            {
                int cost = grid[y][x].cost;
                sf::Color color;

                if (cost <= maxCostValue * 0.1f)
                    color = sf::Color(255, 245, 200); // Light yellow
                else if (cost <= maxCostValue * 0.3f)
                    color = sf::Color(255, 220, 160); // Pale orange
                else if (cost <= maxCostValue * 0.5f)
                    color = sf::Color(255, 190, 120); // Tan
                else if (cost <= maxCostValue * 0.7f)
                    color = sf::Color(255, 160, 90); // Light brown
                else if (cost <= maxCostValue * 0.9f)
                    color = sf::Color(255, 120, 80); // Orange/red
                else
                    color = sf::Color(220, 60, 60); // Muted red

                tileShapes[index].setFillColor(color);
            }
            else
            {
                tileShapes[index].setFillColor(sf::Color(10, 10, 10)); // Default grey colour
            }

            window.draw(tileShapes[index]);

            // Draw UI text for cost/integration values
            if (displayMode != DisplayMode::NONE)
            {
                int displayValue = 0;
                std::string displayStr;

                if (displayMode == DisplayMode::COST_FIELD)
                {
                    displayValue = grid[y][x].cost;
                }
                else if (displayMode == DisplayMode::INTEGRATION_FIELD)
                {
                    displayValue = grid[y][x].integrationCost;
                }

                if (grid[y][x].terrainCost == 255)
                {
                    displayStr = "X"; // Show "X" for obstacles
                }
                else if (displayValue == -1)
                {
                    displayStr = "-"; // Show "-" for unreachable tiles
                }
                else
                {
                    displayStr = std::to_string(displayValue);
                }

                costText.setString(displayStr);

                // Center the text in the tile
                sf::FloatRect textBounds = costText.getLocalBounds();
                costText.setOrigin(sf::Vector2f(textBounds.position.x + textBounds.size.x / 2.0f, textBounds.position.y + textBounds.size.y / 2.0f));
                costText.setPosition(sf::Vector2f(x * tileSize + tileSize / 2.0f, y * tileSize + tileSize / 2.0f));

                window.draw(costText);
            }
        }
    }
    if (showVectorField)
    {
        for (int y = 0; y < gridHeight; y++)
        {
            for (int x = 0; x < gridWidth; x++)
            {
                if (grid[y][x].terrainCost == 255 || grid[y][x].integrationCost == -1)
                    continue;

                createVectorField(window, x, y, grid[y][x].flowDirection);
            }
        }
    }

    window.draw(UIBox);
    window.draw(instructionsText);
}

// Get the vector/flow direction for a given tile
sf::Vector2i FlowField::getFlowDirection(int x, int y) const
{
    // Skip the goal tile
    if (x == static_cast<int>(goalPosition.x) && y == static_cast<int>(goalPosition.y))
        return {0, 0};

    // If unvisited for now, return no direction
    if (grid[y][x].integrationCost == -1)
        return { 0, 0 };

    int lowestCost = INT_MAX;
    sf::Vector2i bestDirection = { 0, 0 };

    for (int i = 0; i < NEIGHBOUR_COUNT; i++)
    {
        int neighbourX = x + DX[i];
        int neighbourY = y + DY[i];

        // Check if neighbor is valid and reachable
        if (isValid(neighbourX, neighbourY) &&
            grid[neighbourY][neighbourX].integrationCost != -1 &&
            grid[neighbourY][neighbourX].terrainCost != 255)
        {
            int neighbourCost = grid[neighbourY][neighbourX].integrationCost;

            // Choose the neighbor with the lowest integration cost
            if (neighbourCost < lowestCost)
            {
                lowestCost = neighbourCost;
                bestDirection = sf::Vector2i(DX[i], DY[i]);
            }
        }
    }

    return bestDirection;
}

// Convert mouse screen position to grid coordinates
sf::Vector2f FlowField::worldToGrid(sf::Vector2f worldPos) const
{
    return sf::Vector2f(static_cast<int>(worldPos.x / tileSize),
                        static_cast<int>(worldPos.y / tileSize));
}

bool FlowField::isValid(int x, int y) const
{
    return x >= 0 && x < gridWidth && y >= 0 && y < gridHeight;
}

bool FlowField::loadFont(const std::string &fontPath)
{
    if (uiFont.openFromFile(fontPath))
    {
        instructionsText.setFont(uiFont);
        costText.setFont(uiFont);
        return true;
    }
    return false;
}

void FlowField::toggleCostField()
{
    if (displayMode == DisplayMode::COST_FIELD)
        displayMode = DisplayMode::NONE;
    else
        displayMode = DisplayMode::COST_FIELD;
}

void FlowField::toggleIntegrationField()
{
    if (displayMode == DisplayMode::INTEGRATION_FIELD)
        displayMode = DisplayMode::NONE;
    else
        displayMode = DisplayMode::INTEGRATION_FIELD;
}

// removed the enum version of this so that heatmap and cost/integration fields can be toggled independently
void FlowField::toggleHeatmap()
{
    showHeatmap = !showHeatmap;
}

void FlowField::toggleVectorField()
{
    showVectorField = !showVectorField;
}
