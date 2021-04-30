// walls.h version 2.0 - Walls class for holding wall coordinates and enum for
// specifying a wall.
// Copyright 2006 Chad Berchek
// Not compatible with Walls version 1.0
// Changes:
//   2.0:
//     - made struct into a class for future compatibility

#ifndef WALLS_H
#define WALLS_H

// x1 is the the wall with the lowest x coordinate, x2 is the
// wall with the highest x coordinate. y1 is the
// wall with the lowest y coordinate and y2 is the wall
// with the highest y coordinate. The terms "top, bottom, left, and
// right" are avoided because their meaning depends on the orientation
// of the coordinate system.
class Walls {
	public:

		// Constructors
		Walls() {
			ix1 = ix2 = iy1 = iy2 = 0.;
		}
		
		Walls(double sx1, double sy1, double sx2, double sy2) {
			ix1 = sx1;
			iy1 = sy1;
			ix2 = sx2;
			iy2 = sy2;
		}
		
		// Get methods
		double x1() const { return ix1; }
		double y1() const { return iy1; }
		double x2() const { return ix2; }
		double y2() const { return iy2; }

		// Set methods
		void setX1(const double sx1) { ix1 = sx1; }
		void setY1(const double sy1) { iy1 = sy1; }
		void setX2(const double sx2) { ix2 = sx2; }
		void setY2(const double sy2) { iy2 = sy2; }

		// Constants
		enum Wall { NONE, X1, Y1, X2, Y2 };

	private:
      double ix1;
		double iy1;
		double ix2;
		double iy2;
};

#endif
