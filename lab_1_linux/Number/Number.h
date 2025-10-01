#pragma once

class Number {
private:
    double value;
public:
    Number() : value(0.0) {}
    Number(double v) : value(v) {}
    
    Number operator+(const Number& other) const {
        return Number(value + other.value);
    }
    Number operator-(const Number& other) const {
        return Number(value - other.value);
    }
    Number operator*(const Number& other) const {
        return Number(value * other.value);
    }
    Number operator/(const Number& other) const {
        return Number(value / other.value);
    }
    
    double getValue() const {
        return value;
    }
};

extern const Number NUM_ZERO;
extern const Number NUM_ONE;
Number createNumber(double v);