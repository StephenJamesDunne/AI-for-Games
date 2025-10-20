#include "FlowField.h"
#include <iostream>

FlowField::FlowField(int w, int h, float size)
    : gridWidth(w), gridHeight(h), tileSize(size), goalPosition(-1, -1)
{
    grid.resize(gridHeight, std::vector<Tile>(gridWidth));
    
    // Initialize tile shapes for visualization
    tileShapes.reserve(gridWidth * gridHeight);
    for (int y = 0; y < gridHeight; ++y)
    {
        for (int x = 0; x < gridWidth; ++x)
        {
            sf::RectangleShape rect(sf::Vector2f(tileSize - 1.0f, tileSize - 1.0f));
            rect.setPosition({ x * tileSize, y * tileSize });
            rect.setFillColor(sf::Color(50, 50, 50));
            rect.setOutlineColor(sf::Color(30, 30, 30));
            rect.setOutlineThickness(0.5f);
            tileShapes.push_back(rect);
        }
    }
}

void FlowField::setGoal(sf::Vector2i goalPos)
{
    if (isValid(goalPos.x, goalPos.y))
    {
        goalPosition = goalPos;
        generateFlowField();
    }
}

void FlowField::setObstacle(sf::Vector2i pos, bool isObstacle)
{
    if (isValid(pos.x, pos.y))
    {
        grid[pos.y][pos.x].traversable = !isObstacle;
    }
}

void FlowField::generateFlowField()
{
    if (goalPosition.x < 0 || goalPosition.y < 0)
        return;
    
    calculateCostField();
    calculateFlowField();
}

void FlowField::calculateCostField()
{
    // Reset costs
    for (int y = 0; y < gridHeight; ++y)
    {
        for (int x = 0; x < gridWidth; ++x)
        {
            grid[y][x].cost = INT_MAX;
        }
    }
    
    // Bushfire Algorithm - propagate costs in waves from goal
    std::queue<sf::Vector2i> currentWave;
    std::queue<sf::Vector2i> nextWave;
    
    grid[goalPosition.y][goalPosition.x].cost = 0;
    currentWave.push(goalPosition);
    
    // 8-directional neighbors
    const int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    const int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    
    int waveCost = 0;
    
    while (!currentWave.empty())
    {
        // Process entire current wave
        while (!currentWave.empty())
        {
            sf::Vector2i current = currentWave.front();
            currentWave.pop();
            
            // Only process if this tile hasn't been assigned a lower cost
            if (grid[current.y][current.x].cost < waveCost)
                continue;
            
            // Spread to all neighbors
            for (int i = 0; i < 8; ++i)
            {
                int nx = current.x + dx[i];
                int ny = current.y + dy[i];
                
                if (!isValid(nx, ny) || !grid[ny][nx].traversable)
                    continue;
                
                // Diagonal movement costs more
                int moveCost = (i % 2 == 0) ? 14 : 10; // 14 ~= sqrt(2)*10
                int newCost = waveCost + moveCost;
                
                // If this is a better path, update and add to next wave
                if (newCost < grid[ny][nx].cost)
                {
                    grid[ny][nx].cost = newCost;
                    nextWave.push(sf::Vector2i(nx, ny));
                }
            }
        }
        
        // Move to next wave
        currentWave = std::move(nextWave);
        nextWave = std::queue<sf::Vector2i>(); // Reset nextWave
        
        // Increment wave cost - using smaller increment for smoother propagation
        waveCost += 10;
    }
}

void FlowField::calculateFlowField()
{
    flowLines.clear();
    
    for (int y = 0; y < gridHeight; ++y)
    {
        for (int x = 0; x < gridWidth; ++x)
        {
            if (!grid[y][x].traversable || grid[y][x].cost == INT_MAX)
            {
                grid[y][x].flowDirection = sf::Vector2f(0.0f, 0.0f);
                continue;
            }
            
            // Find neighbor with lowest cost
            int lowestCost = grid[y][x].cost;
            sf::Vector2f bestDirection(0.0f, 0.0f);
            
            const int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
            const int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};
            
            for (int i = 0; i < 8; ++i)
            {
                int nx = x + dx[i];
                int ny = y + dy[i];
                
                if (isValid(nx, ny) && grid[ny][nx].cost < lowestCost)
                {
                    lowestCost = grid[ny][nx].cost;
                    bestDirection = sf::Vector2f(static_cast<float>(dx[i]), 
                                                 static_cast<float>(dy[i]));
                }
            }
            
            // Normalize direction
            float length = std::sqrt(bestDirection.x * bestDirection.x + 
                                    bestDirection.y * bestDirection.y);
            if (length > 0.0f)
            {
                grid[y][x].flowDirection = bestDirection / length;
            }
            else
            {
                grid[y][x].flowDirection = sf::Vector2f(0.0f, 0.0f);
            }
        }
    }
}

void FlowField::render(sf::RenderWindow& window)
{
    // Draw tiles
    for (int y = 0; y < gridHeight; ++y)
    {
        for (int x = 0; x < gridWidth; ++x)
        {
            int idx = y * gridWidth + x;
            
            if (!grid[y][x].traversable)
            {
                tileShapes[idx].setFillColor(sf::Color(150, 50, 50)); // Red for obstacles
            }
            else if (x == goalPosition.x && y == goalPosition.y)
            {
                tileShapes[idx].setFillColor(sf::Color(50, 200, 50)); // Green for goal
            }
            else if (grid[y][x].cost != INT_MAX)
            {
                // Color based on distance to goal
                float intensity = 1.0f - (grid[y][x].cost / 1000.0f);
                intensity = std::max(0.2f, std::min(1.0f, intensity));
                tileShapes[idx].setFillColor(sf::Color(50, 50, (50 + intensity * 100)));
            }
            else
            {
                tileShapes[idx].setFillColor(sf::Color(50, 50, 50));
            }
            
            window.draw(tileShapes[idx]);
        }
    }
    
    // Draw flow field arrows
    flowLines.clear();
    for (int y = 0; y < gridHeight; ++y)
    {
        for (int x = 0; x < gridWidth; ++x)
        {
            if (grid[y][x].traversable && grid[y][x].cost != INT_MAX &&
                (x != goalPosition.x || y != goalPosition.y))
            {
                sf::Vector2f center(x * tileSize + tileSize / 2.0f, 
                                   y * tileSize + tileSize / 2.0f);
                sf::Vector2f dir = grid[y][x].flowDirection;
                
                if (dir.x != 0.0f || dir.y != 0.0f)
                {
                    sf::Vector2f end = center + dir * (tileSize * 0.3f);
                    
                    flowLines.push_back(sf::Vertex{ {center}, sf::Color::Yellow });
                    flowLines.push_back(sf::Vertex{ {end}, sf::Color::Yellow });
                }
            }
        }
    }
    
    if (!flowLines.empty())
    {
        window.draw(&flowLines[0], flowLines.size(), sf::PrimitiveType::Lines);
    }
}

sf::Vector2f FlowField::getFlowDirection(sf::Vector2f worldPos) const
{
    sf::Vector2i gridPos = worldToGrid(worldPos);
    
    if (isValid(gridPos.x, gridPos.y))
    {
        return grid[gridPos.y][gridPos.x].flowDirection;
    }
    
    return sf::Vector2f(0.0f, 0.0f);
}

bool FlowField::isTraversable(sf::Vector2i gridPos) const
{
    if (isValid(gridPos.x, gridPos.y))
    {
        return grid[gridPos.y][gridPos.x].traversable;
    }
    return false;
}

void FlowField::toggleObstacle(sf::Vector2f worldPos)
{
    sf::Vector2i gridPos = worldToGrid(worldPos);
    
    if (isValid(gridPos.x, gridPos.y))
    {
        // Don't toggle if it's the goal
        if (gridPos.x == goalPosition.x && gridPos.y == goalPosition.y)
            return;
            
        grid[gridPos.y][gridPos.x].traversable = !grid[gridPos.y][gridPos.x].traversable;
        generateFlowField();
    }
}

sf::Vector2i FlowField::worldToGrid(sf::Vector2f worldPos) const
{
    return sf::Vector2i(static_cast<int>(worldPos.x / tileSize),
                       static_cast<int>(worldPos.y / tileSize));
}

bool FlowField::isValid(int x, int y) const
{
    return x >= 0 && x < gridWidth && y >= 0 && y < gridHeight;
}