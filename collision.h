// collision.h - version 2.0
// A class to describe a collision and functions for detecting
// and calculating collisions.
// Copyright 2006 Chad Berchek
// Revisions:
//   2.0:
//     - removed Ball and *Ball from this class, along with getBall1() and getBall2()
//       and removed any method arguments that take Ball

#ifndef COLLISION_H
#define COLLISION_H

#include "walls.h"
#include "ball.h"

// Class to describe a collision. Collisions can be between ball 1 and a wall
// or between balls 1 and 2, or there may be no collision at all.
class Collision {
	public:

		// Constructors
		Collision() { reset(); }
		
		// Get / set methods
		// Signal that there will be a collision with a wall
		// w = which wall it is colliding with (from Walls::Wall enum)
		// t = time until collision
		void setCollisionWithWall(const Walls::Wall w, const double t) {
			whichWall = w;
			timeToCollision = t;
			collisionType = WALL;
		}
		
		// Signal that two balls will collide
		// t = time until collision
		void setCollisionWithBall(const double t) {
			timeToCollision = t;
			collisionType = BALL;
			whichWall = Walls::NONE;
		}
		
		// Clear the current collision settings. Set collision to NONE
		void reset() {
			collisionType = NONE;
			whichWall = Walls::NONE;
			timeToCollision = 0.;
		}
		
		// Will there be a collision?
		bool ball1HasCollision() const { return collisionType != NONE; }
		
		// Will there be a collision with another ball?
		bool ball2HasCollision() const { return collisionType == BALL; }
		
		// Will there be a collision with the wall?
		bool ball1HasCollisionWithWall() const { return collisionType == WALL; }
		
		// Will there be a collision with another ball?
		bool ball1HasCollisionWithBall() const { return collisionType == BALL; }
		
		// With which wall will there be a collision?
		Walls::Wall getCollisionWall() const { return whichWall; }
		
		double getTimeToCollision() const { return timeToCollision; }
		
	private:
		enum Type { NONE, WALL, BALL };
		Type collisionType;
		Walls::Wall whichWall;
		double timeToCollision;
};

// Finds the time until two specified balls collide. If they don't collide,
// the returned Collision will indicate that. If the balls are overlapping
// a collision is NOT detected.
// Implemented in collision.cpp
Collision findTimeUntilTwoBallsCollide(const Ball &b1, const Ball &b2);

// Finds time until specified ball collides with any wall. If they
// don't collide, the returned Collision indicates that. If there
// will be collisions with more than one wall, this function returns
// the earliest collision. IMPORTANT: This function assumes that the
// ball is bounded within the specified walls.
// Implemented in collision.cpp
Collision findTimeUntilBallCollidesWithWall(const Ball &b, const Walls &w);

// Updates the velocities of b1 and b2 to reflect the effect of an elastic
// collision between the two. IMPORTANT: This function does NOT check the
// positions of the balls to see if they're actually colliding. It just
// assumes that they are. Use findTimeUntilTwoBallsCollide() to see
// if the balls are colliding.
// Implemented in collision.cpp
void doElasticCollisionTwoBalls(Ball &b1, Ball &b2);

// Updates the velocity of the ball to reflect the effect of an elastic
// collision with a specified wall. IMPORTANT: This function does NOT
// check to see if the ball and wall are actually colliding. It just
// assumes that they are. Use findTimeUntilBallCollidesWithWall() to
// see if they are colliding. ALSO IMPORTANT: This function assumes
// that the ball is within the area specified by the walls and sets
// the velocities accordingly.
// Implemented in collision.cpp
void doElasticCollisionWithWall(Ball &b, const Walls::Wall w);

#endif
