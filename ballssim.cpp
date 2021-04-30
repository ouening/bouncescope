// ballssim.cpp - version 2.4
// Functions declared in ballssim.h.
// Copyright 2006 Chad Berchek
// See ballssim.h for documentation of functions.
// Revisions:
//   2.0:
//     - modified for compatability with Collsion 2.0
//     - earliest collision functions now take pointer references
//   2.1:
//     - added moveWalls() method
//   2.2:
//     - added max. iteration checking to advanceSim method and corresponding get / set methods in BallsSim class
//   2.3
//     - added min. walls dimensions reporting and detection
//   2.4
//     - made min. walls dimensions report 1 greater than max. ball diameter
//       so balls have some room to bounce
//   2.5
//     - added "resetBalls" method
//     - added maxCollisionsPerFrame functionality
//   2.6
//     - changed findEarliestCollisionOfTwoBalls() to check for case when there are no balls in the simulator
//     - changed references to balls.size() to numBalls()
//     - made minimum area four times the area of the balls

#include "ball.h"
#include "walls.h"
#include "ballssim.h"
#include "collision.h"

void BallsSim::advanceBallPositions(const double dt) {
	for (unsigned long i = 0; i < numBalls(); i++) {
		balls[i].advanceBallPosition(dt);
	}
}

Collision BallsSim::findEarliestCollisionOfTwoBalls(Ball *&b1, Ball *&b2) {
	Collision earliestCollision;
	
	if (numBalls() == 0) return earliestCollision; // Make sure there are some balls
	
	// Compare each pair of balls. Index i runs from the first
	// ball up through the second-to-last ball. For each value of
	// i, index j runs from the ball after i up through the last ball.
	for (unsigned long i = 0; i < numBalls() - 1; i++) {
		for (unsigned long j = i + 1; j < numBalls(); j++) {
			Collision c = findTimeUntilTwoBallsCollide(balls[i], balls[j]);
			if (c.ball1HasCollisionWithBall()) {
				if (!earliestCollision.ball1HasCollision() || c.getTimeToCollision() < earliestCollision.getTimeToCollision()) {
					earliestCollision = c;
					b1 = &balls[i];
					b2 = &balls[j];
				}
			}
		}
	}
	
	return earliestCollision;
}

Collision BallsSim::findEarliestCollisionWithWall(Ball *&b) {
	Collision earliestCollision;
	
	// If there are no walls, return no collision
	if (!hasWalls()) return earliestCollision;
	
	// Check each ball to see if any collide. Store the earliest colliding ball.
	for (unsigned long i = 0; i < numBalls(); i++) {
		Collision c = findTimeUntilBallCollidesWithWall(balls[i], walls);
		if (c.ball1HasCollisionWithWall()) {
			if (!earliestCollision.ball1HasCollision() || c.getTimeToCollision() < earliestCollision.getTimeToCollision()) {
				earliestCollision = c;
				b = &balls[i];
			}
		}
	}
	
	return earliestCollision;
}

Collision BallsSim::findEarliestCollision(Ball *&b1, Ball *&b2) {
	Collision earliestCollision = findEarliestCollisionOfTwoBalls(b1, b2);
	if (hasWalls()) {
		Ball *bCollideWithWall;
		Collision cWalls = findEarliestCollisionWithWall(bCollideWithWall);
		if (cWalls.ball1HasCollisionWithWall()) {
			if (!earliestCollision.ball1HasCollisionWithBall() || (cWalls.getTimeToCollision() < earliestCollision.getTimeToCollision())) {
				earliestCollision = cWalls;
				b1 = bCollideWithWall;
			}
		}
	}
	
	return earliestCollision;
}

void BallsSim::advanceSim(const double dt) {
	double tElapsed = 0.;
	Collision c;
	Collision lastCollision;
	Ball *b1;
	Ball *b2;
	
	for (unsigned int i = 0; i < maxCollisions; i++) {
		// Find earliest collision
		c = findEarliestCollision(b1, b2);
		
		// If no collisions, break
		if (!c.ball1HasCollision()) break;
		
		// Is collision within the time frame?
		// Note: condition is tElapsed + timeToCollision strictly < dt, not <=, because if the two were exactly
		// equal, we would perform the velocity adjustment for collision but not move the balls any more, so the
		// collision could be detected again on the next call to advanceSim().
		if (tElapsed + c.getTimeToCollision() < dt) {
			// Collision is within time frame
			// Advance balls to point of collision
			advanceBallPositions(c.getTimeToCollision());
			// Collision is now occuring. Do collision calculation
			if (c.ball1HasCollisionWithWall()) doElasticCollisionWithWall(*b1, c.getCollisionWall());
			else if (c.ball1HasCollisionWithBall()) doElasticCollisionTwoBalls(*b1, *b2);
			tElapsed += c.getTimeToCollision(); // Move time counter forward
		}
		else break; // Break if collision is not within this frame
		lastCollision = c; // Update lastCollision
	}
	
	// Advance ball positions further if necessary after any collisions to complete the time frame
	advanceBallPositions(dt - tElapsed);
}

void BallsSim::moveBallToWithinBounds(Ball &b) {
	// Check wall X1
	if (b.x() - b.r() < walls.x1()) b.setX(walls.x1() + b.r());
	// Check wall Y1
	if (b.y() - b.r() < walls.y1()) b.setY(walls.y1() + b.r());
	// Check wall X2
	if (b.x() + b.r() > walls.x2()) b.setX(walls.x2() - b.r());
	// Check wall Y2
	if (b.y() + b.r() > walls.y2()) b.setY(walls.y2() - b.r());
}

void BallsSim::moveWalls(const Walls &newWalls) {
	walls = newWalls;
	iHasWalls = true;
	for (unsigned int i = 0; i < numBalls(); i++) {
		moveBallToWithinBounds(balls[i]);
	}
}

double BallsSim::getMinWallDimension(double fixedWallDimension) {
	double minDimension = 0.;
	if (fixedWallDimension > 0.) {
		minDimension = 4. * minArea / fixedWallDimension;
	}
	if (minDimension <= maxDiameter) minDimension = maxDiameter + 1;
	return minDimension;
}

void BallsSim::addBall(const Ball &newBall) {
	balls.push_back(newBall);
	balls.back().setID(nextID);
	nextID++;
	maxCollisions = maxCollisionsPerBall * numBalls();
	moveBallToWithinBounds(balls.back());
	if (newBall.r() * 2. > maxDiameter) maxDiameter = newBall.r() * 2.;
	minArea += 4. * newBall.r() * newBall.r(); // Add area of square surrounding ball to minArea
}
