#include <cmath>
#include "Vector.h"

const Vector VECTOR_ZERO = Vector(NUM_ZERO, NUM_ZERO);
const Vector VECTOR_ONES = Vector(NUM_ONE, NUM_ONE);

Number Vector::length() const {
    double dx = x.getValue();
    double dy = y.getValue();
    double len = std::sqrt(dx*dx + dy*dy);
    return Number(len);
}

double Vector::angle() const {
    double dx = x.getValue();
    double dy = y.getValue();
    return std::atan2(dy, dx);
}

Vector Vector::add(const Vector& other) const {
    Number newX = x + other.x;
    Number newY = y + other.y;
    return Vector(newX, newY);
}