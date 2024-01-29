//
// Created by constantin on 13/09/23.
//

#include "Box.h"

/*
namespace Engine {

    const double Box::epsilon = 1.0e-5; //!< Epsilon value used to check intersections and some round off errors.
    const Box Box::Null(0.0); //!< Null box, equivalent to: \code Box(glm::vec3(0.0)); \endcode 

    const int Box::edge[24] =
            {
                    0, 1, 2, 3, 4, 5, 6, 7,
                    0, 2, 1, 3, 4, 6, 5, 7,
                    0, 4, 1, 5, 2, 6, 3, 7
            };

    const glm::vec3 Box::normal[6] =
            {
                    glm::vec3(-1.0, 0.0, 0.0),
                    glm::vec3(0.0, -1.0, 0.0),
                    glm::vec3(0.0, 0.0, -1.0),
                    glm::vec3(1.0, 0.0, 0.0),
                    glm::vec3(0.0, 1.0, 0.0),
                    glm::vec3(0.0, 0.0, 1.0)
            };


    Box::Box(const glm::vec3 &c, double r) {
        a = c - glm::vec3(r);
        b = c + glm::vec3(r);
    }


    Box::Box(const glm::vec3 &a, const glm::vec3 &b) {
        Box::a = a;
        Box::b = b;
    }


    Box::Box(double r) {
        a = -glm::vec3(r);
        b = glm::vec3(r);
    }

    Box::Box(const std::vector<glm::vec3> &v) {
        for (int j = 0; j < 3; j++) {
            a[j] = v.at(0)[j];
            b[j] = v.at(0)[j];
            for (int i = 1; i < v.size(); i++) {
                if (v.at(i)[j] < a[j]) {
                    a[j] = v.at(i)[j];
                }
                if (v.at(i)[j] > b[j]) {
                    b[j] = v.at(i)[j];
                }
            }
        }
    }

    Box::Box(const Box &x, const Box &y) {
        a = {std::min(x.a.x, y.a.x),
             std::min(x.a.y, y.a.y),
             std::min(x.a.z, y.a.z)};
        b = {std::max(x.b.x, y.b.x),
             std::max(x.b.y, y.b.y),
             std::max(x.b.z, y.b.z)};
    }


    Box Box::sub(int n) const {
        glm::vec3 c = center();
        return Box(glm::vec3((n & 1) ? c[0] : a[0], (n & 2) ? c[1] : a[1], (n & 4) ? c[2] : a[2]),
                   glm::vec3((n & 1) ? b[0] : c[0], (n & 2) ? b[1] : c[1], (n & 4) ? b[2] : c[2]));
    }


    void Box::translate(const glm::vec3 &t) {
        a += t;
        b += t;
    }


    void Box::scale(double s) {
        a *= s;
        b *= s;

        // Swap coordinates for negative coefficients 
        if (s < 0.0) {
            glm::vec3 t = a;
            a = b;
            b = t;
        }
    }


//! Returns either end vertex of the box.
    glm::vec3 &Box::operator[](int i) {
        if (i == 0) return a;
        else return b;
    }

//! Overloaded.
    glm::vec3 Box::operator[](int i) const {
        if (i == 0) return a;
        else return b;
    }

//! Returns the center of the box.
    glm::vec3 Box::center() const {
        return 0.5f * (a + b);
    }

    glm::vec3 Box::diagonal() const {
        return (b - a);
    }


    glm::vec3 Box::size() const {
        return b - a;
    }


    double Box::radius() const {
        return 0.5 * glm::length(b - a);
    }


    glm::vec3 Box::vertex(int k) const {
        return {(k & 1) ? b[0] : a[0], (k & 2) ? b[1] : a[1], (k & 4) ? b[2] : a[2]};
    }

//! Compute the volume of a box.
    double Box::volume() const {
        glm::vec3 side = b - a;
        return side[0] * side[1] * side[2];
    }

    double Box::area() const {
        glm::vec3 side = b - a;
        return 2.0 * (side[0] * side[1] + side[0] * side[2] + side[1] * side[2]);
    }


    bool Box::isInside(const Box &box) const {
        return (isInside(box.a) && isInside(box.b));
    }


    bool Box::isInside(const glm::vec3 &p) const {
        return (
                (a.x < p.x) &&
                (a.y < p.y) &&
                (a.z < p.z) &&
                (b.x > p.x) &&
                (b.y > p.y) &&
                (b.z > p.z)
        );
    }


    int operator==(const Box &a, const Box &b) {
        return (a.a == b.a) && (a.b == b.b);
    }


    int operator!=(const Box &a, const Box &b) {
        return !(a == b);
    }
}
*/
namespace Engine {
    glm::vec3 Box::diagonal() {
        return (b - a);
    }

//! Returns either end vertex of the box.
    glm::vec3 &Box::operator[](int i) {
        if (i == 0) return a;
        else return b;
    }

    Box::Box(const glm::vec3 &a, const glm::vec3 &b) : a(a), b(b) {
        computeVertices();
    }

    void Box::computeVertices() {
        vertices[0]=a;
        vertices[1]=glm::vec3(b.x,a.y,a.z);
        vertices[2]=glm::vec3(a.x,a.y,b.z);
        vertices[3]=glm::vec3(a.x,b.y,a.z);
        vertices[4]=b;
        vertices[5]=glm::vec3(b.x,b.y,a.z);
        vertices[6]=glm::vec3(b.x,a.y,b.z);
        vertices[7]=glm::vec3(a.x,b.y,b.z);
    }

    const std::array<glm::vec3, 8> &Box::getVertices() const {
        return vertices;
    }

    bool Box::isInside(const glm::vec3 &p) const {
        return a.x <= p.x && p.x <= b.x && a.y <= p.y && p.y <= b.y;
    }

    const glm::vec3 &Box::getLower() {
        return a;
    }

    const glm::vec3 &Box::getUpper() {
        return b;
    }

    Box::Box(): a({-0.5f,-0.5f,-0.5f}), b({-0.5f,-0.5f,-0.5f}){
        computeVertices();
    }
}


