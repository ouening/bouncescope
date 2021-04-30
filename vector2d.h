// Declarations for 2-D vectors
// Copyright 2006 Chad Berchek
// Version 1.1
// Revisions:
//   1.1: (compatible with 1.0)
//     - added operator-

#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>

class Vector2D {
	public:

		// Constructors
		Vector2D() : internalX(0.), internalY(0.) { }
		Vector2D(double ix, double iy) : internalX(ix), internalY(iy) { }

		// Get / set methods
		double x() const { return internalX; }
		double y() const { return internalY; }
		void setX(const double sx) { internalX = sx; }
		void setY(const double sy) { internalY = sy; }
		void setXY(const double sx, const double sy) { setX(sx); setY(sy); }
		
		// Member functions
		// Get magnitude of vector
		double magnitude() const {
			return std::sqrt(x() * x() + y() * y());
		}

		// Get a unit vector in the direction of this vector
		// If this vector is the 0 vector, return a 0 vector
		Vector2D unitVector() const {
			double mag = magnitude();
			if (mag != 0.) return Vector2D(x() / mag, y() / mag);
			else return Vector2D();
		}

		// Operators
		// Addition
		Vector2D operator+(const Vector2D &right) const {
			return Vector2D(x() + right.x(), y() + right.y());
		}
		
		// Subtraction
		Vector2D operator-(const Vector2D &right) const {
			return Vector2D(x() - right.x(), y() - right.y());
		}
		
		// Dot product
		double operator*(const Vector2D &right) const {
			return (x() * right.x() + y() * right.y());
		}
		
	private:
		double internalX;
		double internalY;
};

// Multiplication by a scalar (c * vector)
inline Vector2D operator*(const double c, const Vector2D &right) {
	return Vector2D(c * right.x(), c * right.y());
}

// Multiplication by a scalar (vector * c)
inline Vector2D operator*(const Vector2D &left, const double c) {
	return c * left; // Call operator*(double, Vector2D&) above
}

#endif
