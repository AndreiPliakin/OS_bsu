#pragma once
#include "Number.h"

#ifdef _WIN32
    #ifdef VECTORLIB_EXPORTS
        #define VECTOR_API __declspec(dllexport)
    #else
        #define VECTOR_API __declspec(dllimport)
    #endif
#else
    #define VECTOR_API
#endif

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