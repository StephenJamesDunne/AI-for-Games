#include "FlowField.h"
#include <iostream>
#include <string>
#include <queue>
#include <cmath>

FlowField::FlowField(int w, int h, float size)
    : gridWidth(w), gridHeight(h), tileSize(size)
{
    grid.resize(gridHeight, std::vector<Tile>(gridWidth));

    tileShapes.reserve(gridWidth * gridHeight);
    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            sf::RectangleShape rect(sf::Vector2f(tileSize - 2.0f, tileSize - 2.0f));
            rect.setPosition({ x * tileSize + UI_WIDTH, y * tileSize });
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

    instructionsText.setCharacterSize(48);
    instructionsText.setFillColor(sf::Color(220, 220, 220));
    instructionsText.setOutlineColor(sf::Color::Black);
    instructionsText.setOutlineThickness(2.0f);
    instructionsText.setPosition({ 10.0f, 10.0f });
    instructionsText.setString(
        "Flowfield Pathfinding\n\nInstructions:\n\n"
        " - Set goal node\n\twith left click\n\n"
        " - Set start node\n\twith right click\n\n"
        " - Toggle obstacle\n\twith middle click\n\n"
        " - Start/Reset NPC\n\twith 'Spacebar'\n\n"
		" NOTE:\n\tNPC only moves if\n\tstart and goal set\n\n"
        " - Toggle cost field\n\twith '1'\n\n"
        " - Toggle heatmap\n\twith '2'\n\n"
        " - Toggle integration\n\twith '3'\n\n"
        " - Toggle vector field\n\twith '4'\n\n"
    );

    costText.setCharacterSize(28);
    costText.setFillColor(sf::Color::White);
    costText.setOutlineColor(sf::Color::Black);
    costText.setOutlineThickness(1.0f);

    npc.setRadius(tileSize / 3.0f);
    npc.setFillColor(sf::Color::Cyan);
    npc.setOutlineColor(sf::Color::Black);
    npc.setOutlineThickness(2.0f);
    npc.setOrigin(sf::Vector2f(npc.getRadius(), npc.getRadius()));

    initializeTerrain();
}

void FlowField::initializeTerrain()
{
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

void FlowField::createCostField()
{
    // Reset all path distances
    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            grid[y][x].cost = -1;
        }
    }

    // Validate goal position
    if (!isValid(goalPosition.x, goalPosition.y) ||
        grid[goalPosition.y][goalPosition.x].terrainCost == 255)
    {
        return;
    }

    grid[goalPosition.y][goalPosition.x].cost = 0;
    maxCostValue = 0;

    // BFS to generate costs
    std::queue<sf::Vector2i> validTiles;
    validTiles.push(goalPosition);

    while (!validTiles.empty())
    {
        sf::Vector2i current = validTiles.front();
        validTiles.pop();

        int currentCost = grid[current.y][current.x].cost;

        for (int i = 0; i < NEIGHBOUR_COUNT; i++)
        {
            int neighbourX = current.x + DX[i];
            int neighbourY = current.y + DY[i];

            if (!isValid(neighbourX, neighbourY))
                continue;

            if (grid[neighbourY][neighbourX].terrainCost == 255)
                continue;

            if (isDiagonalBlocked(current.x, current.y, neighbourX, neighbourY))
                continue;

            // BUSHFIRE: All neighbouring tiles get +1 regardless of direction
            if (grid[neighbourY][neighbourX].cost == -1)
            {
                grid[neighbourY][neighbourX].cost = currentCost + 1;

                if (grid[neighbourY][neighbourX].cost > maxCostValue)
                {
                    maxCostValue = grid[neighbourY][neighbourX].cost;
                }

                validTiles.push(sf::Vector2i(neighbourX, neighbourY));
            }
        }
    }
}

void FlowField::createIntegrationField()
{
    // Validate goal position
    if (!isValid(goalPosition.x, goalPosition.y))
        return;

    // Calculate integration costs: cost field + Euclidean distance to goal
    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            // Skip unreachable tiles and obstacles
            if (grid[y][x].cost == -1 || grid[y][x].terrainCost == 255)
            {
                grid[y][x].integrationCost = -1.0f;
                grid[y][x].flowDirection = { 0, 0 };
                continue;
            }

            // Calculate Euclidean distance from this tile to goal
            float dx = static_cast<float>(x - goalPosition.x);
            float dy = static_cast<float>(y - goalPosition.y);
            float euclideanDist = std::sqrt(dx * dx + dy * dy);

            // Integration = cost field + Euclidean distance (scaled for visibility)
			// cost field is in steps, so scale by tileSize to match Euclidean distance scale
            grid[y][x].integrationCost = grid[y][x].cost * tileSize + static_cast<int>(euclideanDist * tileSize);
        }
    }

    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            if (grid[y][x].integrationCost >= 0.0f)
            {
                grid[y][x].flowDirection = getFlowDirection(x, y);
            }
        }
    }
}

void FlowField::createFlowArrows(sf::RenderWindow& window, int x, int y, sf::Vector2i direction) const
{
    if (direction.x == 0 && direction.y == 0)
        return;

    sf::Vector2f start = getTileCenter(x, y);
    sf::Vector2f end = start + sf::Vector2f(direction.x, direction.y) * (tileSize / 2.5f);

    // Draw main arrow line
    sf::VertexArray arrow(sf::PrimitiveType::Lines, 2);
    arrow[0].position = start;
    arrow[0].color = sf::Color::White;
    arrow[1].position = end;
    arrow[1].color = sf::Color::White;
    window.draw(arrow);

    // Calculate and draw arrowhead
    sf::Vector2f arrowDir = normalizeVector(end - start);
    sf::Vector2f perpendicular(-arrowDir.y, arrowDir.x);

    float headLength = tileSize / 6.0f;
    float headWidth = tileSize / 8.0f;

    sf::Vector2f headPoint1 = end - arrowDir * headLength + perpendicular * headWidth;
    sf::Vector2f headPoint2 = end - arrowDir * headLength - perpendicular * headWidth;

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

void FlowField::setGoal(sf::Vector2f worldPos)
{
    if (mouseIsInUI(worldPos))
        return;
    
    sf::Vector2i gridPos = worldToGrid(worldPos);
    
    if (!isValid(gridPos.x, gridPos.y))
        return;
    
    if (tileIsObstacle(gridPos.x, gridPos.y))
        return;
    
    if (gridPos != startPosition)
    {
        goalPosition = gridPos;
        createCostField();
        createIntegrationField();
        calculateShortestPath();
    }
}

void FlowField::setStart(sf::Vector2f worldPos)
{
    if (mouseIsInUI(worldPos))
        return;

    sf::Vector2i gridPos = worldToGrid(worldPos);

    if (!isValid(gridPos.x, gridPos.y))
        return;

    if (tileIsObstacle(gridPos.x, gridPos.y))
        return;

    if (gridPos != goalPosition)
    {
        startPosition = gridPos;
		calculateShortestPath();
    }
}

void FlowField::toggleObstacle(sf::Vector2f worldPos)
{
    if (mouseIsInUI(worldPos))
        return;

    sf::Vector2i gridPos = worldToGrid(worldPos);

    if (!isValid(gridPos.x, gridPos.y))
        return;
    
    if (gridPos == startPosition || gridPos == goalPosition)
        return;

    // Toggle obstacle state
    if (grid[gridPos.y][gridPos.x].terrainCost == 255)
    {
        grid[gridPos.y][gridPos.x].terrainCost = 1; // Make normal tile
    }
    else
    {
        grid[gridPos.y][gridPos.x].terrainCost = 255; // Make obstacle
    }
    
    if (isValid(startPosition.x, startPosition.y) &&
        isValid(goalPosition.x, goalPosition.y))
    {
        createCostField();
        createIntegrationField();
        calculateShortestPath();
    }
}

void FlowField::render(sf::RenderWindow& window)
{
    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            int index = y * gridWidth + x;

            // Determine tile color
            if (x == goalPosition.x && y == goalPosition.y)
            {
                tileShapes[index].setFillColor(sf::Color(50, 200, 50)); // Green for goal
            }
            else if (x == startPosition.x && y == startPosition.y)
            {
                tileShapes[index].setFillColor(sf::Color(50, 50, 200)); // Blue for start
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
                    color = sf::Color(255, 245, 200);
                else if (cost <= maxCostValue * 0.3f)
                    color = sf::Color(255, 220, 160);
                else if (cost <= maxCostValue * 0.5f)
                    color = sf::Color(255, 190, 120);
                else if (cost <= maxCostValue * 0.7f)
                    color = sf::Color(255, 160, 90);
                else if (cost <= maxCostValue * 0.9f)
                    color = sf::Color(255, 120, 80);
                else
                    color = sf::Color(220, 60, 60);

                tileShapes[index].setFillColor(color);
            }
            else
            {
                tileShapes[index].setFillColor(sf::Color(10, 10, 10)); // Default grey
            }

            window.draw(tileShapes[index]);

            // Draw cost/integration values if display mode is active
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
                    displayValue = static_cast<int>(grid[y][x].integrationCost);
                }

                if (grid[y][x].terrainCost == 255)
                {
                    displayStr = "X";
                }
                else if (displayValue == -1)
                {
                    displayStr = "-";
                }
                else
                {
                    displayStr = std::to_string(displayValue);
                }

                costText.setString(displayStr);

                // Center the text in the tile
                sf::FloatRect textBounds = costText.getLocalBounds();
                costText.setOrigin(sf::Vector2f(
                    textBounds.position.x + textBounds.size.x / 2.0f,
                    textBounds.position.y + textBounds.size.y / 2.0f));
                costText.setPosition(getTileCenter(x, y));

                window.draw(costText);
            }
        }
    }

    // Draw vector field arrows
    if (showVectorField)
    {
        for (int y = 0; y < gridHeight; y++)
        {
            for (int x = 0; x < gridWidth; x++)
            {
                if (tileIsReachable(x, y))
                {
                    createFlowArrows(window, x, y, grid[y][x].flowDirection);
                }
            }
        }
    }

    drawShortestPath(window);

    // Draw NPC
    if (npcActive)
    {
        window.draw(npc);
    }

    // Draw UI
    window.draw(UIBox);
    window.draw(instructionsText);
}

sf::Vector2i FlowField::getFlowDirection(int x, int y) const
{
    // Goal tile has no direction
    if (x == goalPosition.x && y == goalPosition.y)
        return { 0, 0 };

    // Unreachable tiles have no direction
    if (grid[y][x].integrationCost < 0.0f)
        return { 0, 0 };

    int bestDirection = -1;
    float bestCost = 9999999.0f;
    float bestEuclidean = 999999.0f;

    // Find neighbour with lowest integration cost
    for (int i = 0; i < NEIGHBOUR_COUNT; i++)
    {
        int neighbourX = x + DX[i];
        int neighbourY = y + DY[i];

        if (!isValid(neighbourX, neighbourY))
            continue;

        if (!tileIsReachable(neighbourX, neighbourY))
            continue;

        if (isDiagonalBlocked(x, y, neighbourX, neighbourY))
            continue;

        float neighbourCost = grid[neighbourY][neighbourX].integrationCost;

        // Update Euclidean distance for a potential tiebreaker
        float dx = static_cast<float>(neighbourX - goalPosition.x);
        float dy = static_cast<float>(neighbourY - goalPosition.y);
        float euclideanDist = dx * dx + dy * dy;

		// If this neighbouring tile has a lower cost, or same cost but closer to goal, move to it
        if (neighbourCost < bestCost)
        {
            bestCost = neighbourCost;
            bestDirection = i;
			bestEuclidean = euclideanDist;
            
        }
        else if (neighbourCost == bestCost && euclideanDist < bestEuclidean)
        {
            bestDirection = i;
            bestEuclidean = euclideanDist;
        }
    }

    if (bestDirection != -1)
    {
        return sf::Vector2i(DX[bestDirection], DY[bestDirection]);
    }

    return { 0, 0 };
}

void FlowField::resetNPC()
{
    if (isValid(startPosition.x, startPosition.y) && shortestPath.size() > 0)
    {
		currentPathIndex = 0;
        npcPos = sf::Vector2f(static_cast<float>(shortestPath[0].x),
            static_cast<float>(shortestPath[0].y));
        npcActive = true;
    }
    else
    {
		npcActive = false;
    }
}

void FlowField::calculateShortestPath()
{
    shortestPath.clear();

	// Make sure start and goal are valid first
    if (!isValid(startPosition.x, startPosition.y) ||
        !isValid(goalPosition.x, goalPosition.y))
    {
        return;
    }

    sf::Vector2i currentPos = startPosition;
	shortestPath.push_back(currentPos);

    int maxSteps = gridWidth * gridHeight; // Prevent infinite loops
    int steps = 0;

    while (currentPos != goalPosition && steps < maxSteps)
    {
        // Get the flow direction for current tile
        sf::Vector2i flowDir = grid[currentPos.y][currentPos.x].flowDirection;

        // If no flow direction, path is invalid
        if (flowDir.x == 0 && flowDir.y == 0)
        {
            break;
        }

        // Move to next tile
        currentPos.x += flowDir.x;
        currentPos.y += flowDir.y;

        // Validate new position
        if (!isValid(currentPos.x, currentPos.y) || tileIsObstacle(currentPos.x, currentPos.y))
        {
            break;
        }

        shortestPath.push_back(currentPos);
        steps++;
    }

    if (currentPos != goalPosition)
    {
		// Need to clear the vector if pathfinding to goal node failed
        shortestPath.clear(); 
	}
}

void FlowField::drawShortestPath(sf::RenderWindow& window)
{
    if (shortestPath.size() < 2)
    {
        return;
    }

    // Draw path as connected line segments
    for (size_t i = 0; i < shortestPath.size() - 1; i++)
    {
        sf::Vector2f start = getTileCenter(shortestPath[i].x, shortestPath[i].y);
        sf::Vector2f end = getTileCenter(shortestPath[i + 1].x, shortestPath[i + 1].y);

        // Draw thick yellow line
        sf::VertexArray line(sf::PrimitiveType::Lines, 2);
        line[0].position = start;
        line[0].color = sf::Color(255, 255, 0, 180); // Semi-transparent yellow
        line[1].position = end;
        line[1].color = sf::Color(255, 255, 0, 180);

        window.draw(line);
    }

    // Draw dots at each path node for clarity
    for (const auto& node : shortestPath)
    {
        sf::CircleShape dot(tileSize / 8.0f);
        dot.setFillColor(sf::Color(255, 255, 0, 220));
        dot.setOutlineColor(sf::Color(200, 200, 0));
        dot.setOutlineThickness(1.0f);
        dot.setOrigin(sf::Vector2f(dot.getRadius(), dot.getRadius()));
        dot.setPosition(getTileCenter(node.x, node.y));
        window.draw(dot);
    }
}

void FlowField::findPath(sf::Time deltaTime)
{
    if (!npcActive)
        return;

    // Check if NPC has reached the goal
    if (currentPathIndex >= static_cast<int>(shortestPath.size()))
    {
        npcActive = false;
        return;
    }

    sf::Vector2i targetTile = shortestPath[currentPathIndex];
    sf::Vector2f targetPos(static_cast<float>(targetTile.x),
        static_cast<float>(targetTile.y));

    sf::Vector2f direction = targetPos - npcPos;
    float distanceToTarget = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    float movement = NPC_SPEED * deltaTime.asSeconds();

    if (movement >= distanceToTarget)
    {
        npcPos = targetPos;
        currentPathIndex++;  // Move to next tile in path
    }
    else
    {
        // Move toward target
        sf::Vector2f normalizedDirection = normalizeVector(direction);
        npcPos += normalizedDirection * movement;
    }

    // Update visual position
    npc.setPosition(sf::Vector2f(
        npcPos.x * tileSize + tileSize / 2.0f + UI_WIDTH,
        npcPos.y * tileSize + tileSize / 2.0f));

}

bool FlowField::isDiagonalBlocked(int fromX, int fromY, int toX, int toY) const
{
	// Only check for diagonal moves from (fromX, fromY) to (toX, toY)
    int dx = toX - fromX;
    int dy = toY - fromY;

	// Ignore non-diagonal moves
    if (dx == 0 || dy == 0)
        return false;

    // Check the two adjacent cells that the diagonal crosses
    // For a diagonal move, both adjacent cells need to be passable
    bool horizontalBlocked = tileIsObstacle(fromX + dx, fromY);
    bool verticalBlocked = tileIsObstacle(fromX, fromY + dy);

    // Diagonal is blocked if either adjacent cell is an obstacle
    return horizontalBlocked || verticalBlocked;
}

// Helper functions
sf::Vector2i FlowField::worldToGrid(sf::Vector2f worldPos) const
{
    return sf::Vector2i(static_cast<int>((worldPos.x - UI_WIDTH) / tileSize),
        static_cast<int>(worldPos.y / tileSize));
}

sf::Vector2f FlowField::gridToWorld(int x, int y) const
{
    return sf::Vector2f(x * tileSize + UI_WIDTH, y * tileSize);
}

sf::Vector2f FlowField::getTileCenter(int x, int y) const
{
    return sf::Vector2f(x * tileSize + tileSize / 2.0f + UI_WIDTH,
        y * tileSize + tileSize / 2.0f);
}

bool FlowField::isValid(int x, int y) const
{
    return x >= 0 && x < gridWidth && y >= 0 && y < gridHeight;
}

bool FlowField::mouseIsInUI(sf::Vector2f mousePos) const
{
    return mousePos.x < UI_WIDTH;
}

bool FlowField::tileIsObstacle(int x, int y) const
{
    return grid[y][x].terrainCost == 255;
}

bool FlowField::tileIsReachable(int x, int y) const
{
    return !tileIsObstacle(x, y) && grid[y][x].integrationCost >= 0.0f;
}

sf::Vector2f FlowField::normalizeVector(sf::Vector2f vec) const
{
    float length = std::sqrt(vec.x * vec.x + vec.y * vec.y);
    if (length > 0.0f)
    {
        return sf::Vector2f(vec.x / length, vec.y / length);
    }
    return sf::Vector2f(0.0f, 0.0f);
}

bool FlowField::loadFont(const std::string& fontPath)
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

void FlowField::toggleHeatmap()
{
    showHeatmap = !showHeatmap;
}

void FlowField::toggleVectorField()
{
    showVectorField = !showVectorField;
}