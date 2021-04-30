// ball.h version 2.0
// Copyright 2006 Chad Berchek
// Not compatible with version 1.0
// Replaces Ball structure 1.0
// Changes:
//   2.0:
//     - made struct into a class for future compatibility between versions
//     - changed position and velocity to vectors
//     - eliminated need to #include windows.h
//     - added ID member

#ifndef BALL_H
#define BALL_H

#include "vector2d.h"

// Documentation on Ball:
// Ball has several properties which you can get / set with the appropriate methods.
// Properties:
// x and y position (x, y, pos (vector))
// x and y velocity (vx, vy, v (vector))
// mass (m)
// radius (r)
// color (color)
// id (id) - an integer used to identify the ball if there are several balls in a container

class Ball {
	public:

		// Constructors
		Ball() {
			im = 0.;
			ir = 0.;
			icolor = 0;
			// Vectors should initialize themselves to <0, 0>
		}
		
		// Get methods
		double x() const { return ipos.x(); }
		double y() const { return ipos.y(); }
		double vx() const { return iv.x(); }
		double vy() const { return iv.y(); }
		double m() const { return im; }
		double r() const { return ir; }
		unsigned long color() const { return icolor; }
		const Vector2D &pos() const { return ipos; }
		const Vector2D &v() const { return iv; }
		int id() const { return iid; }
		
		// Set methods
		void setX(const double x) { ipos.setX(x); }
		void setY(const double y) { ipos.setY(y); }
		void setXY(const double x, const double y) { setX(x); setY(y); }
		void setPos(const Vector2D &pos) { ipos = pos; }
		void setVX(const double vx) { iv.setX(vx); }
		void setVY(const double vy) { iv.setY(vy); }
		void setVXY(const double vx, const double vy) { setVX(vx); setVY(vy); }
		void setV(const Vector2D &v) { iv = v; }
		void setM(const double mass) { im = mass; }
		void setR(const double radius) { ir = radius; }
		void setColor(const unsigned long color) { icolor = color; }
		void setID(const int sid) { iid = sid; }
		
		// Other methods
		// Moves the ball according to the current velocity by time dt
		void advanceBallPosition(const double dt) {
			setX(x() + vx() * dt);
			setY(y() + vy() * dt);
		}
		
	private:
		// Note: i stands for internal
		Vector2D ipos; // Position
		Vector2D iv; // Velocity
		double im; // Mass
		double ir; // Radius
		unsigned long icolor; // Color
		int iid; // ID
};

#endif
