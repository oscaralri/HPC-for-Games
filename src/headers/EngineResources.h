#pragma once

#include "TextureManager.h"
#include "ModelManager.h"

class EngineResources
{
public:
    static TextureManager& GetTextureManager()
    {
        static TextureManager textureManager;
        return textureManager;
    }

    static ModelManager& GetModelManager() 
    {
        static ModelManager modelManager;
        return modelManager;
    }

private:
    EngineResources() = default;
};
