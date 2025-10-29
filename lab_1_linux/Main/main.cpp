#include <iostream>
#include <cmath>
#include "Number.h"
#include "Vector.h"

int main() {
    Number a = createNumber(5.0);
    Number b = NUM_ONE;
    Number c = a + b;
    std::cout << "Number example: " 
              << a.getValue() << " + " << b.getValue() 
              << " = " << c.getValue() << std::endl;

    Vector v1(NUM_ONE, createNumber(2.0));
    Vector v2(createNumber(3.0), createNumber(4.0));
    Vector sum = v1.add(v2);
    
    std::cout << "Vector v1 = ("
              << v1.getX().getValue() << ", " << v1.getY().getValue() << ")"
              << std::endl;
    std::cout << "Vector v2 = ("
              << v2.getX().getValue() << ", " << v2.getY().getValue() << ")"
              << std::endl;
    std::cout << "v1 + v2 = ("
              << sum.getX().getValue() << ", " << sum.getY().getValue() << ")"
              << std::endl;
              
    Number r = sum.length();
    double phi = sum.angle();
    std::cout << "Length of (v1+v2) = " << r.getValue() << std::endl;
    std::cout << "Angle of (v1+v2) = " << phi << " radians" << std::endl;
    
    return 0;
}