//
// Created by constantin on 19/01/24.
//

#include "Image.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "../../libraries/bgfx.cmake/bimg/3rdparty/stb/stb_image.h"
namespace Engine{
    Image::Image(std::string &fileName) {
        loadFromFile(fileName);
    }

    bool Image::loadFromFile(std::string &fileName) {
        int channels;
        uint8_t * data = stbi_load(fileName.c_str(), &_width, &_height, &channels, 4);
        _size = _width*_height;
        if (data != nullptr){
            _data = std::vector<uint8_t>(data,data + _size*4);
        }
        stbi_image_free(data);
        return data!= nullptr;
    }

    glm::vec4 Image::at(int x, int y) {
        int index = (x+y*_width)*4;
        return {_data[index],_data[index+1],_data[index+2],_data[index+3]};
    }

    int Image::getWidth() const {
        return _width;
    }

    int Image::getHeight() const {
        return _height;
    }

    int Image::getSize() const {
        return _size;
    }

    const std::vector<uint8_t> &Image::getData() const {
        return _data;
    }



}