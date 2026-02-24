#pragma once

#include "TextureManager.h"
#include "ModelManager.h"
#include "ShaderManager.h"
#include "MDI.h"

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

    static MDI& GetMDI()
    {
        static MDI mdi;
        return mdi;
    }

private:
    EngineResources() = default;
};
