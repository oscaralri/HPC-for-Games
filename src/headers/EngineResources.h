#pragma once

#include "TextureManager.h"
#include "ModelManager.h"
#include "ShaderManager.h"

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

    static ShaderManager& GetShaderManager()
    {
        static ShaderManager shaderManager;
        return shaderManager;
    }

private:
    EngineResources() = default;
};
