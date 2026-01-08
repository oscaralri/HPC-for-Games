#pragma once

#include <vector>
#include "ECSConfig.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_int3.hpp>
#include <glm/common.hpp>

struct GridCell
{
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
        cellsX = worldSize.x / cellSize.x;
        cellsY = worldSize.y / cellSize.y;
        cellsZ = worldSize.z / cellSize.z;

        int totalCells = cellsX * cellsY * cellsZ;
        cells.resize(totalCells);
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
                    int index = x + y * cellsX + z * cellsX * cellsY;
                    cells[index].entities.push_back(entity);
                }
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





// algoritmo
	// tener escena subdividida en GridPartition, tener lista? de todas en Grid
		// será algo muy sencillo a la hora de que estarán definidos los límites manualmente y tal
	// comprobar con GridSystem qué entidades están dentro de qué GridPartitions y meterlas 
		// son estáticas así que no tengo que preocuparme de cambios

// frustum culling
	// primero comprobar sobre los gridCell (comprubeas qué gridcell colisiona con la cámara)
		// cuando entras dentro de un grid partition ya compruebas dentro de él

