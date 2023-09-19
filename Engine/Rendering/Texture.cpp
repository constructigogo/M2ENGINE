//
// Created by constantin on 19/09/23.
//

#include "Texture.h"

namespace Engine {
    int Texture::getWidth() const {
        return width;
    }

    int Texture::getHeight() const {
        return height;
    }

    const bgfx::TextureHandle &Texture::getHandle() const {
        return handle;
    }

    Texture::Texture(const std::string &name, int width, int height, const bgfx::TextureHandle &handle) : name(name),
                                                                                                          width(width),
                                                                                                          height(height),
                                                                                                          handle(handle) {}

    const std::string &Texture::getName() const {
        return name;
    }
} // Engine