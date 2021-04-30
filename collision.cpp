// collision.cpp version 1.0
// Implementation of collision functions.
// Copyright 2006 Chad Berchek
// See collision.h for documentation of what these functions do.

#include "collision.h"
#include "vector2d.h"
#include "walls.h"
#include "ball.h"
#include <cmath>
using namespace std;

// Utility function to compute x squared
inline double square(double x) { return x * x; }

Collision findTimeUntilTwoBallsCollide(const Ball &b1, const Ball &b2) {
	Collision clsn;
	
	// Compute parts of quadratic formula
	// a = (v2x - v1x) ^ 2 + (v2y - v1y) ^ 2
	double a = square(b2.vx() - b1.vx()) + square(b2.vy() - b1.vy());
	// b = 2 * ((x20 - x10) * (v2x - v1x) + (y20 - y10) * (v2y - v1y))
	double b = 2. * ((b2.x() - b1.x()) * (b2.vx() - b1.vx()) + (b2.y() - b1.y()) * (b2.vy() - b1.vy()));
	// c = (x20 - x10) ^ 2 + (y20 - y10) ^ 2 - (r1 + r2) ^ 2
	double c = square(b2.x() - b1.x()) + square(b2.y() - b1.y()) - square(b1.r() + b2.r());
	
	// Determinant = b^2 - 4ac
	double det = square(b) - 4 * a * c;
	
	if (a != 0.) { // If a == 0 then v2x==v1x and v2y==v1y and there will be no collision
		double t = (-b - sqrt(det)) / (2. * a); // Quadratic formula. t = time to collision
		if (t >= 0.) { // If collision occurs...
			clsn.setCollisionWithBall(t);
		}
	}
	
	return clsn;
}

Collision findTimeUntilBallCollidesWithWall(const Ball &b, const Walls &w) {
	double timeToCollision = 0.;
	Walls::Wall whichWall = Walls::NONE;
	Collision clsn;
	
	// Check for collision with wall X1
	if (b.vx() < 0.) {
		double t = (b.r() - b.x() + w.x1()) / b.vx();
		if (t >= 0.) { // If t < 0 then ball is headed away from wall
			timeToCollision = t;
			whichWall = Walls::X1;
		}
	}
	
	// Check for collision with wall Y1
	if (b.vy() < 0.) {
		double t = (b.r() - b.y() + w.y1()) / b.vy();
		if (t >= 0.) {
			if (whichWall == Walls::NONE || t < timeToCollision) {
				timeToCollision = t;
				whichWall = Walls::Y1;
			}
		}
	}
	
	// Check for collision with wall X2
	if (b.vx() > 0.) {
		double t = (w.x2() - b.r() - b.x()) / b.vx();
		if (t >= 0.) {
			if (whichWall == Walls::NONE || t < timeToCollision) {
				timeToCollision = t;
				whichWall = Walls::X2;
			}
		}
	}
	
	// Check for collision with wall Y2
	if (b.vy() > 0.) {
		double t = (w.y2() - b.r() - b.y()) / b.vy();
		if (t >= 0.) {
			if (whichWall == Walls::NONE || t < timeToCollision) {
				timeToCollision = t;
				whichWall = Walls::Y2;
			}
		}
	}
	
	// Setup Collision return value
	if (whichWall != Walls::NONE) { // If there is a collision...
		clsn.setCollisionWithWall(whichWall, timeToCollision);
	}
	
	return clsn;
}

void doElasticCollisionTwoBalls(Ball &b1, Ball &b2) {
	// Avoid division by zero below in computing new normal velocities
	// Doing a collision where both balls have no mass makes no sense anyway
	if (b1.m() == 0. && b2.m() == 0.) return;

	// Compute unit normal and unit tangent vectors
	Vector2D v_n = b2.pos() - b1.pos(); // v_n = normal vec. - a vector normal to the collision surface
	Vector2D v_un = v_n.unitVector(); // unit normal vector
	Vector2D v_ut(-v_un.y(), v_un.x()); // unit tangent vector
	
	// Compute scalar projections of velocities onto v_un and v_ut
	double v1n = v_un * b1.v(); // Dot product
	double v1t = v_ut * b1.v();
	double v2n = v_un * b2.v();
	double v2t = v_ut * b2.v();
	
	// Compute new tangential velocities
	double v1tPrime = v1t; // Note: in reality, the tangential velocities do not change after the collision
	double v2tPrime = v2t;
	
	// Compute new normal velocities using one-dimensional elastic collision equations in the normal direction
	// Division by zero avoided. See early return above.
	double v1nPrime = (v1n * (b1.m() - b2.m()) + 2. * b2.m() * v2n) / (b1.m() + b2.m());
	double v2nPrime = (v2n * (b2.m() - b1.m()) + 2. * b1.m() * v1n) / (b1.m() + b2.m());
	
	// Compute new normal and tangential velocity vectors
	Vector2D v_v1nPrime = v1nPrime * v_un; // Multiplication by a scalar
	Vector2D v_v1tPrime = v1tPrime * v_ut;
	Vector2D v_v2nPrime = v2nPrime * v_un;
	Vector2D v_v2tPrime = v2tPrime * v_ut;
	
	// Set new velocities in x and y coordinates
	b1.setVX(v_v1nPrime.x() + v_v1tPrime.x());
	b1.setVY(v_v1nPrime.y() + v_v1tPrime.y());
	b2.setVX(v_v2nPrime.x() + v_v2tPrime.x());
	b2.setVY(v_v2nPrime.y() + v_v2tPrime.y());
}

void doElasticCollisionWithWall(Ball &b, const Walls::Wall w) {
	switch (w) {
		case (Walls::X1):
			b.setVX(fabs(b.vx()));
			break;
		case (Walls::Y1):
			b.setVY(fabs(b.vy()));
			break;
		case (Walls::X2):
			b.setVX(-fabs(b.vx()));
			break;
		case (Walls::Y2):
			b.setVY(-fabs(b.vy()));
			break;
	}
}
