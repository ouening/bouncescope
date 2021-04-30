// ballssim.h - version 2.6
// Class for a simulator of many balls
// Copyright 2006 Chad Berchek
// Revisions - see ballssim.cpp

#ifndef BALLSSIM_H
#define BALLSSIM_H

#include "ball.h"
#include "walls.h"
#include "collision.h"
#include <vector>

class BallsSim {
	public:

		// Constructors
		BallsSim() {
			iHasWalls = false;
			maxCollisionsPerBall = 10;
			resetBalls();
		}

		// Modifier methods
		// Replace internal set of balls with setBalls
		void setBallsVector(const std::vector<Ball> &setBalls) {
			balls = setBalls;
		}
		
		// Remove all balls and reset counters
		void resetBalls() {
			balls.clear(); // Delete all balls from vector
			nextID = 0; // Reset ID counter
			minArea = 0.;
			maxDiameter = 0.;
			maxCollisions = 10; // This will be overwritten on the first call to addBall()
		}
		
		// Set maximum number of collisions for a frame based on the number of balls
		void setMaxCollisionsPerBall(unsigned int cpb) {
			maxCollisionsPerBall = cpb;
		}
		
		// Apply boundaries to simulation
		void addWalls(const Walls &w) {
			moveWalls(w);
		}
		
		// Moves the walls. Moves balls, without collision checking,
		// to be within the new boundaries
		void moveWalls(const Walls &newWalls);

		// Remove boundaries of simulation
		void removeWalls() {
			iHasWalls = false;
		}
		
		// Adds a ball to the simulation. Ball ID is set
		// automatically and newBall.ID is ignored.
		void addBall(const Ball &newBall);
		
		// Advances the simulation by time dt with full
		// collision detection
		void advanceSim(const double dt);
		
		// Other methods
		// Finds earliest of any collisions - between balls or
		// with walls (if walls present).
		Collision findEarliestCollision(Ball *&b1, Ball *&b2);
		
		// Get the max. number of collisions per frame based on the number of balls
		unsigned int getMaxCollisionsPerBall() const { return maxCollisionsPerBall; }
		
		// Get the minimum dimension of the walls in one direction given the other dimension
		// This depends on the area occupied by the balls and the diameter of the largest ball
		double getMinWallDimension(double fixedWallDimension);
		
		// Wall boundaries have been set?
		bool hasWalls() const { return iHasWalls; }
		
		// How many balls are there?
		unsigned long numBalls() const { return balls.size(); }
		
		// getBall - no bounds checking. Caller must use numBalls() to make sure index is valid
		// Index starts at 0 and runs up through numBalls()-1
		const Ball &getBall(unsigned long index) const { return balls[index]; }

	private:
		std::vector<Ball> balls; // Stores all the balls
		bool iHasWalls; // Have wall boundaries been set?
		Walls walls; // Wall boundaries
		int nextID; // Next ID to assign to an added ball
		unsigned int maxCollisions; // Max number of collisions per frame in advanceSim
		unsigned int maxCollisionsPerBall; // Max number of collisions per frame based on the number of balls
		double minArea; // Minimum area within walls
		double maxDiameter; // Maximum diameter out of all the balls
		
		// Advances ball positions according to current velocities
		// with no collision detection. Advances by time dt
		void advanceBallPositions(const double dt);
		
		// Look at all pairs of balls and find the earliest
		// collision between any two.
		Collision findEarliestCollisionOfTwoBalls(Ball *&b1, Ball *&b2);
		
		// Look at all balls and find the earliest one
		// to collide with a wall.
		Collision findEarliestCollisionWithWall(Ball *&b);
		
		// Moves a ball, which may be anywhere, to within the walls
		void moveBallToWithinBounds(Ball &b);
};

#endif
