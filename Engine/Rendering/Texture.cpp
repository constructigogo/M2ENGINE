//
// Created by constantin on 19/09/23.
//

#include <cassert>
#include "Texture.h"

namespace Engine {
    int Texture::getWidth() const {
        return width;
    }

    int Texture::getHeight() const {
        return height;
    }

    const bgfx::TextureHandle &Texture::getHandle() const {
        assert(bgfx::isValid(handle));
        return handle;
    }

    const std::string &Texture::getName() const {
        return name;
    }

    Texture::Texture(const std::string &name, int width, int height,
                     const bgfx::TextureHandle &handle, Texture::TYPE texType) : name(name), texType(texType), width(width), height(height),
                                                          handle(handle) {}

    Texture::TYPE Texture::getType() const {
        return texType;
    }
} // Engine