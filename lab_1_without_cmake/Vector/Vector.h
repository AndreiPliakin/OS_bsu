#pragma once

#ifdef _WIN32
#ifdef VECTORLIB_EXPORTS
#define NUMBER_API 
#define VECTOR_API __declspec(dllexport)
#else
#define NUMBER_API __declspec(dllimport)
#define VECTOR_API __declspec(dllimport)
#endif
#else
#define NUMBER_API
#define VECTOR_API
#endif


#include "../Number/Number.h"

class VECTOR_API Vector {
private:
    Number x;
    Number y;
public:
    Vector() : x(NUM_ZERO), y(NUM_ZERO) {}
    Vector(const Number& xVal, const Number& yVal) : x(xVal), y(yVal) {}

    Number getX() const { return x; }
    Number getY() const { return y; }

    Number length() const;
    double angle() const;
    Vector add(const Vector& other) const;
};

extern VECTOR_API const Vector VECTOR_ZERO;
extern VECTOR_API const Vector VECTOR_ONES;