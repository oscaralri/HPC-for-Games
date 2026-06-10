#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <random>

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
    RandomGenerator(int size, unsigned int seed, float minX, float maxX, 
        float minY, float maxY, float minZ, float maxZ, const std::vector<ExclusionZone>& zonesX, const std::vector<ExclusionZone>& zonesZ)
        : counterBding(0), counterCarX(0), counterCarZ(0)
    {
        GenerateRandBuildings(size, seed, minX, maxX, minY, maxY, minZ, maxZ, zonesX, zonesZ);
    }
  
    const glm::vec3& GetPositionBding()
    {
        if (counterBding >= positionsBdings.size())
            counterBding = 0; 

        return positionsBdings[counterBding++];
    }

    const glm::vec3& GetPositionCarsX()
    {
        if (counterCarX >= positionsCarsX.size())
            counterCarX = 0; 

        return positionsCarsX[counterCarX++];
    }

    const glm::vec3& GetPositionCarsZ()
    {
        if (counterCarZ >= positionsCarsZ.size())
            counterCarZ = 0; 

        return positionsCarsZ[counterCarZ++];
    }

private:
    std::vector<glm::vec3> positionsBdings;
    std::vector<glm::vec3> positionsCarsX;
    std::vector<glm::vec3> positionsCarsZ;

    size_t counterBding;
    size_t counterCarX;
    size_t counterCarZ;

    void GenerateRandBuildings(int size, unsigned int seed, float minX, 
        float maxX, float minY, float maxY, float minZ, float maxZ, const std::vector<ExclusionZone>& zonesX, const std::vector<ExclusionZone>& zonesZ)
    {
        positionsBdings.reserve(size);
        positionsCarsX.reserve(size);
        positionsCarsZ.reserve(size);

        std::mt19937 generator(seed);
        std::uniform_real_distribution<float> distX(minX, maxX);
        std::uniform_real_distribution<float> distY(minY, maxY);
        std::uniform_real_distribution<float> distZ(minZ, maxZ);

        for (int i = 0; i < size; i++)
        {
            float x = distX(generator);
            float z = distZ(generator);

            bool inZoneX = false;
            bool inZoneZ = false;
            for (const auto& zone : zonesX)
            {
                if (zone.Contains(x, z))
                {
                    inZoneX = true;
                    break;
                }
            }

            for (const auto& zone : zonesZ)
            {
                if (zone.Contains(x, z))
                {
                    inZoneZ = true;
                    break;
                }
            }

            if (inZoneX)
            {
                positionsCarsX.emplace_back(x, distY(generator), z);
            }
            else if (inZoneZ)
            {
                positionsCarsZ.emplace_back(x, distY(generator), z);
            }
            else
            {
                positionsBdings.emplace_back(x, distY(generator), z);
            }
        }
    }
};
