#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <random>

enum RandType
{
    RandBuilding, RandCar
};

struct ExclusionZone {
    float minX, maxX;
    float minZ, maxZ;

    bool Contains(float x, float z) const {
        return (x >= minX && x <= maxX && z >= minZ && z <= maxZ);
    }
};

class RandomGenerator
{
public:
    RandomGenerator(RandType randType, int size, unsigned int seed, float minX, float maxX, 
        float minY, float maxY, float minZ, float maxZ, const std::vector<ExclusionZone>& zones = {})
        : counterBding(0), counterCar(0)
    {
        if(randType == RandBuilding) GenerateRandBuildings(size, seed, minX, maxX, minY, maxY, minZ, maxZ, zones);
        if (randType == RandCar) GenerateRandCar(size, seed, minX, maxX, minY, maxY, minZ, maxZ, zones);
    }
    /*
    const std::vector<glm::vec3>& GetPositions() const { return positions; }

    const glm::vec3& GetPosition()
    {
        if (counter >= positions.size())
            counter = 0; // evitar errores basico

        return positions[counter++];
    }
    */
    const glm::vec3& GetPositionBding()
    {
        if (counterBding >= positionsBdings.size())
            counterBding = 0; // evitar errores basico

        return positionsBdings[counterBding++];
    }

    const glm::vec3& GetPositionCars()
    {
        if (counterCar >= positionsCars.size())
            counterCar = 0; // evitar errores basico

        return positionsCars[counterCar++];
    }

private:
    std::vector<glm::vec3> positionsBdings;
    std::vector<glm::vec3> positionsCars;
    size_t counterBding;
    size_t counterCar;

    void GenerateRandBuildings(int size, unsigned int seed, float minX, 
        float maxX, float minY, float maxY, float minZ, float maxZ, const std::vector<ExclusionZone>& zones)
    {
        positionsBdings.reserve(size);
        positionsCars.reserve(size);

        std::mt19937 generator(seed);
        std::uniform_real_distribution<float> distX(minX, maxX);
        std::uniform_real_distribution<float> distY(minY, maxY);
        std::uniform_real_distribution<float> distZ(minZ, maxZ);
        /*
        while (positionsBdings.size() < size)
        {
            float x = distX(generator);
            float z = distZ(generator);

            bool inForbiddenZone = false;
            for (const auto& zone : zones) {
                if (zone.Contains(x, z)) {
                    inForbiddenZone = true;
                    break;
                }
            }

            if (!inForbiddenZone) {
                positions.emplace_back(x, distY(generator), z);
            }
        }
        */

        for (int i = 0; i < size; i++)
        {
            float x = distX(generator);
            float z = distZ(generator);

            bool inForbiddenZone = false;
            for (const auto& zone : zones) {
                if (zone.Contains(x, z)) {
                    inForbiddenZone = true;
                    break;
                }
            }

            if (!inForbiddenZone) {
                positionsBdings.emplace_back(x, distY(generator), z);
            }
            else
            {
                positionsCars.emplace_back(x, distY(generator), z);
            }
        }
    }

    void GenerateRandCar(int size, unsigned int seed, float minX, 
        float maxX, float minY, float maxY, float minZ, float maxZ, const std::vector<ExclusionZone>& zones)
    {
        positionsCars.reserve(size);

        std::mt19937 generator(seed);
        std::uniform_real_distribution<float> distX(minX, maxX);
        std::uniform_real_distribution<float> distY(minY, maxY);
        std::uniform_real_distribution<float> distZ(minZ, maxZ);


        for (int i = 0; i < size; ++i)
        {
            positionsCars.emplace_back(distX(generator), distY(generator), distZ(generator));
        }
    }
};
