//
// Created by constantin on 20/12/23.
//

#pragma once

#include <vector>
#include <assert.h>
namespace Engine {

    template<typename T>
    class Grid {
    public:
        Grid(int sizeX, int sizeY, T defaultValue) : _sizeX(sizeX), _sizeY(sizeY), _defaultValue(defaultValue) {
            _gridData.resize(_sizeX * _sizeY, _defaultValue);
        }

        inline int index(int x, int y) const{
            assert(isInside(x,y));
            return x + y * _sizeX;
        }

        T &at(int x, int y) {
            assert(isInside(x,y));
            return _gridData[index(x, y)];
        }

        const T& at(int x, int y) const {
            assert(isInside(x,y));
            return _gridData[index(x, y)];
        }

        void setAt(T value,int x, int y){
            assert(isInside(x,y));
            _gridData[index(x,y)]=value;
        }

        inline bool isInside(int x, int y) const{
            return x >= 0 && x < _sizeX && y >= 0 && y < _sizeY;
        }

        inline size_t length() const{
            return _sizeX*_sizeY;
        }

        inline size_t gridWidth()const {
            return _sizeX;
        }

        inline size_t gridHeight()const {
            return _sizeY;
        }
    protected:
        int _sizeX;
        int _sizeY;
        T _defaultValue;

        std::vector<T> _gridData;

    private:
    };

} // Engine
