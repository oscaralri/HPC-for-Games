#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <random>

class RandomGenerator
{
public:
    RandomGenerator(
        int size,
        unsigned int seed,
        float minX, float maxX,
        float minY, float maxY,
        float minZ, float maxZ
    )
        : counter(0)
    {
        Generate(size, seed, minX, maxX, minY, maxY, minZ, maxZ);
    }

    const std::vector<glm::vec3>& GetPositions() const { return positions; }

    const glm::vec3& GetPosition()
    {
        if (counter >= positions.size())
            counter = 0; // evitar errores basico

        return positions[counter++];
    }

private:
    std::vector<glm::vec3> positions;
    size_t counter;

    void Generate(
        int size,
        unsigned int seed,
        float minX, float maxX,
        float minY, float maxY,
        float minZ, float maxZ
    )
    {
        positions.reserve(size);

        std::mt19937 generator(seed);
        std::uniform_real_distribution<float> distX(minX, maxX);
        std::uniform_real_distribution<float> distY(minY, maxY);
        std::uniform_real_distribution<float> distZ(minZ, maxZ);

        for (int i = 0; i < size; ++i)
        {
            positions.emplace_back(
                distX(generator),
                distY(generator),
                distZ(generator)
            );
        }
    }
};
