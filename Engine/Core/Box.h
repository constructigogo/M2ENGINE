//
// Created by constantin on 13/09/23.
//

#pragma once

#include "glm/vec3.hpp"
#include <array>

namespace Engine {

    class Box {
    public:
        Box();
        Box(const glm::vec3 &a, const glm::vec3 &b);

        glm::vec3 diagonal();
        bool isInside(const glm::vec3 &p) const;
        const std::array<glm::vec3, 8> &getVertices() const;

        const glm::vec3 & getLower();
        const glm::vec3 & getUpper();

        glm::vec3 &operator[](int i);
    protected:
        void computeVertices();
        glm::vec3 a, b; //!< Lower and upper vertex.
        std::array<glm::vec3,8> vertices;
    };

/*
#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#include <compare>



    class Box {
    protected:
        glm::vec3 a, b; //!< Lower and upper vertex.
    public:
        //! Empty.
        Box() {}

        explicit Box(double);

        explicit Box(const glm::vec3 &, const glm::vec3 &);

        explicit Box(const glm::vec3 &, double);

        explicit Box(const std::vector<glm::vec3> &);

        explicit Box(const Box &, const Box &);

        //! Empty.
        ~Box() {}

        // Access vertexes
        glm::vec3 &operator[](int);

        glm::vec3 operator[](int) const;

        // Comparison
        friend int operator==(const Box &, const Box &);

        friend int operator!=(const Box &, const Box &);

        // Acces to vertices
        glm::vec3 center() const;

        glm::vec3 vertex(int) const;

        glm::vec3 size() const;

        glm::vec3 diagonal() const;

        double radius() const;

        bool isInside(const Box &) const;

        bool isInside(const glm::vec3 &) const;

        double volume() const;

        double area() const;

        // Compute sub-box
        Box sub(int) const;

        // Translation, scale
        void translate(const glm::vec3 &);

        void scale(double);
    public:
        static const double epsilon; //!< Internal \htmlonly\epsilon;\endhtmlonly for ray intersection tests.
        static const Box Null; //!< Empty box.
        static const int edge[24]; //!< Edge vertices.
        static const glm::vec3 normal[6]; //!< Face normals.
    };
    */
}
