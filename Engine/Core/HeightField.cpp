//
// Created by constantin on 20/12/23.
//

#include "HeightField.h"

namespace Engine {
    float HeightField::heightAt(int x, int y) const {
        return at(x,y) * scale;
    }

    float HeightField::getScale() const {
        return scale;
    }

    glm::vec3 HeightField::normalAt(int x, int y) const {
        glm::vec3 startX;
        glm::vec3 startY;
        glm::vec3 endX;
        glm::vec3 endY;

        glm::vec3 resX;
        glm::vec3 resY;
        if (x == 0) {
            startX = glm::vec3(x, heightAt(x, y), y);
            endX = glm::vec3(x+1, heightAt(x+1, y), y);
        } else if (x==_sizeX - 1) {
            startX = glm::vec3(x-1, heightAt(x-1, y), y);
            endX = glm::vec3(x, heightAt(x, y), y);
        }
        else {
            startX = glm::vec3(x-1, heightAt(x-1, y), y);
            endX = glm::vec3(x+1, heightAt(x+1, y), y);
        }

        if (y == 0) {
            startY = glm::vec3(x, heightAt(x, y), y);
            endY = glm::vec3(x, heightAt(x, y+1), y+1);
        } else if (y==_sizeY - 1) {
            startY = glm::vec3(x, heightAt(x, y-1), y-1);
            endY = glm::vec3(x, heightAt(x, y), y);
        }
        else {
            startY = glm::vec3(x, heightAt(x, y-1), y-1);
            endY = glm::vec3(x, heightAt(x, y+1), y+1);
        }

        glm::vec3 X = glm::normalize(endX - startX);
        glm::vec3 Y = glm::normalize(endY - startY);
        glm::vec3 res = glm::normalize(glm::cross(X,-Y));
        //res = glm::vec3(res.x,res.z,res.y);

        return res;
    }


} // Engine