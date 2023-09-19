//
// Created by constantin on 19/09/23.
//

#pragma once
#include "bgfx/bgfx.h"
#include <string>

namespace Engine {

    class Texture {
    public:
        Texture() = default;

        Texture(const std::string &name, int width, int height, const bgfx::TextureHandle &handle);

        enum TYPE {
            COLOR,
            NORMAL,
            AO,
            DEPTH
        };

        const bgfx::TextureHandle &getHandle() const;

        const std::string &getName() const;

        int getWidth() const;
        int getHeight() const;

    protected:
        std::string name;
        int width;
        int height;
        bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;


    private:
    };

} // Engine
