#include "FlowField.h"
#include <iostream>
#include <string>
#include <queue>

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
            rect.setPosition({ x * tileSize, y * tileSize });
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
    instructionsText.setPosition({ 10.0f, 10.0f });
    instructionsText.setString(
        "Flowfield Pathfinding\n\nInstructions:\n\n\nSet goal node\nwith left click\n\nSet start node\nwith right click\n\nToggle cost field\nwith '1'"
    );

    costText.setCharacterSize(14);
    costText.setFillColor(sf::Color::White);
    costText.setOutlineColor(sf::Color::Black);
    costText.setOutlineThickness(1.0f);

    createCostField();
}

// Step 1: Create Cost Field
// Set base cost of travelling to each tile
void FlowField::createCostField()
{
    // Initialize all tiles with default traversal cost
    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
			grid[y][x].cost = 1; // Normal passable tile
        }
    }

    // Create a vertical wall
    for (int y = 10; y < 20; y++)
    {
        grid[y][25].cost = 255;
    }

    // Create a horizontal wall
    for (int x = 10; x < 20; x++)
    {
        grid[25][x].cost = 255;
    }
}

// Step 2: Calculate Integration Field
void FlowField::createIntegrationField()
{
	// Reset all values in case goal changes
    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            grid[y][x].integrationCost = -1;
        }
    }

    // Check if goal is valid
    if (goalPosition.x < 0 || goalPosition.y < 0 || !isValid(static_cast<int>(goalPosition.x), static_cast<int>(goalPosition.y)))
    {
        return;
    }

    int goalX = static_cast<int>(goalPosition.x);
    int goalY = static_cast<int>(goalPosition.y);

	// Goal can't be on an obstacle
    if (grid[goalY][goalX].cost == 255)
    {
        return;
    }

    // Bushfire algorithm:
	// Use a queue and start from goal node, explore neighbors in breadth-first search
	// Check all 8 directions from current tile for valid neighbors
	// Update their integration cost if -1 (unvisited tile), or found a shorter path to goal
	// Push valid neighbors to queue for further exploration

    std::queue<sf::Vector2f> validTiles;

    grid[goalY][goalX].integrationCost = 0;
    validTiles.push(sf::Vector2f(goalX, goalY));

	// All 8 possible neighbor directions
	// Up, Down, Left, Right, and 4 Diagonals
    const int dx[] = { 0, 0, -1, 1, -1, 1, -1, 1 };
    const int dy[] = { -1, 1, 0, 0, -1, -1, 1, 1 };

    while (!validTiles.empty())
    {
        sf::Vector2f current = validTiles.front();
        validTiles.pop();

        int currentX = current.x;
        int currentY = current.y;
        int currentIntegration = grid[currentY][currentX].integrationCost;

        for (int i = 0; i < 8; ++i)
        {
            int neighborX = currentX + dx[i];
            int neighborY = currentY + dy[i];

            // Skip if not a valid tile (outside the bounds of the grid height/width, or under UI)
            if (!isValid(neighborX, neighborY))
            {
                continue;
            }

            // Skip if obstacle
            if (grid[neighborY][neighborX].cost == 255)
            {
                continue;
            }

            // Calculate new integration value
            int newIntegration = currentIntegration + grid[neighborY][neighborX].cost;

            // If unvisited OR we found a shorter path
            if (grid[neighborY][neighborX].integrationCost == -1 || newIntegration < grid[neighborY][neighborX].integrationCost)
            {
                grid[neighborY][neighborX].integrationCost = newIntegration;
                validTiles.push(sf::Vector2f(neighborX, neighborY));
            }
        }
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

    if (isValid(gridPos.x, gridPos.y))
    {
        if (gridPos.x != startPosition.x || gridPos.y != startPosition.y)
        {
            goalPosition = gridPos;
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

    if (isValid(gridPos.x, gridPos.y))
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
    if (isValid(gridPos.x, gridPos.y))
    {
        return grid[static_cast<int>(gridPos.y)][static_cast<int>(gridPos.x)].cost == 255;
    }
    return false;
}

void FlowField::render(sf::RenderWindow& window)
{
    for (int y = 0; y < gridHeight; ++y)
    {
        for (int x = 0; x < gridWidth; ++x)
        {
            int index = y * gridWidth + x;

            if (x == goalPosition.x && y == goalPosition.y)
            {
                tileShapes[index].setFillColor(sf::Color(50, 200, 50)); // Green for goal node
            }
            else if (x == startPosition.x && y == startPosition.y)
            {
                tileShapes[index].setFillColor(sf::Color(50, 50, 200)); // Blue for start node
            }
            else if (grid[y][x].cost == 255)
            {
                tileShapes[index].setFillColor(sf::Color(255, 0, 0)); // Dark grey for obstacles
			}
            else
            {
                tileShapes[index].setFillColor(sf::Color(10, 10, 10)); // Default grey colour
            }

            window.draw(tileShapes[index]);

            // Draw cost values if cost field is toggled on
            if (showCostField)
            {
                int integrationValue = grid[y][x].integrationCost;
                std::string displayStr;

                if (grid[y][x].cost == 255)
                {
                    displayStr = "X"; // Show "X" for obstacles
                }
                else if (integrationValue == -1)
                {
                    displayStr = "-"; // Show "-" for unreachable tiles
                }
                else
                {
                    displayStr = std::to_string(integrationValue);
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

	window.draw(UIBox);
	window.draw(instructionsText);
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

bool FlowField::loadFont(const std::string& fontPath)
{
    if (uiFont.openFromFile(fontPath))
    {
        instructionsText.setFont(uiFont);
        return true;
    }
    return false;
}

void FlowField::toggleCostField()
{
    showCostField = !showCostField;
}