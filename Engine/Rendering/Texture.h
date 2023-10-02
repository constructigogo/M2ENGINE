//
// Created by constantin on 19/09/23.
//

#pragma once
#include "bgfx/bgfx.h"
#include <string>

namespace Engine {

    class Texture {
    public:

        enum TYPE {
            COLOR,
            NORMAL,
            SPECULAR,
            AO,
            DEPTH
        };

        Texture() = default;

        Texture(const std::string &name,int width, int height, const bgfx::TextureHandle &handle ,TYPE texType);


        const bgfx::TextureHandle &getHandle() const;

        const std::string &getName() const;

        int getWidth() const;
        int getHeight() const;
        TYPE getType() const;


    protected:
        std::string name;
        TYPE texType;
        int width;
        int height;
        bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;


    private:
    };

} // Engine
