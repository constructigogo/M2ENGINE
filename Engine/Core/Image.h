//
// Created by constantin on 19/01/24.
//

#pragma once
#include <string>
#include <vector>
#include "glm/vec4.hpp"

namespace Engine {
    class Image {
    public:
        Image() = default;
        Image(std::string & fileName);

        glm::vec4 at(int x, int y);

        bool loadFromFile(std::string &fileName);

        int getWidth() const;

        int getHeight() const;

        int getSize() const;

        const std::vector<uint8_t> &getData() const;

    protected:
        int _width;
        int _height;
        int _size;
        std::vector<uint8_t> _data;
    private:
    };
}

