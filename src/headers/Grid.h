#pragma once

#include <vector>
#include "ECSConfig.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_int3.hpp>
#include <glm/common.hpp>

struct GridCell
{
    glm::vec3 min;
    glm::vec3 max;
    std::vector<ECS::Entity> entities;
};

struct Grid
{
    glm::vec3 origin;
    glm::vec3 cellSize;

    int cellsX = 0;
    int cellsY = 0;
    int cellsZ = 0;

    std::vector<GridCell> cells;

    Grid(const glm::vec3& origin_, const glm::vec3& worldSize, const glm::vec3& cellSize_) 
        : origin(origin_), cellSize(cellSize_)
    {
        // esto es el numero de celdas en cada eje
        cellsX = worldSize.x / cellSize.x; 
        cellsY = worldSize.y / cellSize.y;
        cellsZ = worldSize.z / cellSize.z;

        int totalCells = cellsX * cellsY * cellsZ;
        cells.resize(totalCells);

        for (int z = 0; z < cellsZ; ++z)
        {
            for (int y = 0; y < cellsY; ++y)
            {
                for (int x = 0; x < cellsX; ++x)
                {
                    int index = GetIndex(x, y, z);

                    glm::vec3 cellMin = origin + glm::vec3(x, y, z) * cellSize;
                    glm::vec3 cellMax = cellMin + cellSize;

                    cells[index].min = cellMin;
                    cells[index].max = cellMax;
                }
            }
        }
    }

    int GetIndex(int x, int y, int z) const
    {
        return x + y * cellsX + z * cellsX * cellsY;
    }

    void Insert(ECS::Entity entity, const glm::vec3& min, const glm::vec3& max)
    {
        glm::ivec3 minCell = ClampCell(WorldToCell(min));
        glm::ivec3 maxCell = ClampCell(WorldToCell(max));

        for (int z = minCell.z; z <= maxCell.z; ++z)
        {
            for (int y = minCell.y; y <= maxCell.y; ++y)
            {
                for (int x = minCell.x; x <= maxCell.x; ++x)
                {
                    int index = GetIndex(x, y, z);
                    cells[index].entities.push_back(entity);                }
            }
        }    
    }

    glm::ivec3 ClampCell(const glm::ivec3& c) const
    {
        return glm::clamp(c, glm::ivec3(0), glm::ivec3(cellsX - 1, cellsY - 1, cellsZ - 1));
    }

    glm::ivec3 WorldToCell(const glm::vec3& pos) const
    {
        glm::vec3 rel = pos - origin;

        return glm::ivec3(int(rel.x / cellSize.x), int(rel.y / cellSize.y), int(rel.z / cellSize.z));
    }
};