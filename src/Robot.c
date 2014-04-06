/* Copyright 2014 Neil Edelman, distributed under the terms of the
 GNU General Public License, see copying.txt */

/** this is the example file that shows how to make a policy using Policy.c
 and Square.c
 @author Neil */

#include <stdlib.h> /* malloc free */
#include <stdio.h>  /* fprintf */
#include "Square.h"
#include "Policy.h"

/* constants */
static const char *programme   = "Policy";
static const char *year        = "2014";
static const int versionMajor  = 0;
static const int versionMinor  = 0;

/* the no of cells in our example 1 */
#define N (11)
/* the no of cells in example 2 <= example 1 */
#define N2 (6)

/* private */
static double compute_transition(const enum Direction dir, const struct Square *from, const struct Square *to);
static double compute_transition_2(const enum Direction dir, const struct Square *from, const struct Square *to);
static void usage(const char *argvz);

/** private (entry point) */
int main(int argc, char **argv) {
	struct Square *state[N];
	struct Policy *p;
	double reward;
	int i;
	int convergence;

	/* we don't accept command line args */
	if(argc > 1) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	/* this is to get states like this:
	 0 0 0 1
	 0   0 -10
	 0 0 0 0   */
	
	for(i = 0; i < N; i++) {
		reward = 0;
		if(i == 3) reward = 1;
		else if(i == 6) reward = -10;
		state[i] = Square(reward, UP);
	}

	SquareConnect(state[0], state[1], LEFT, -1);
	SquareConnect(state[1], state[2], LEFT, -1);
	SquareConnect(state[2], state[3], LEFT, 0);

	SquareConnect(state[0], state[4], DOWN, -1);
	SquareConnect(state[2], state[5], DOWN, -1);

	SquareConnect(state[5], state[6], LEFT, 0);

	SquareConnect(state[4], state[7], DOWN, -1);
	SquareConnect(state[5], state[9], DOWN, -1);

	SquareConnect(state[7], state[8], LEFT, -1);
	SquareConnect(state[8], state[9], LEFT, -1);
	SquareConnect(state[9], state[10],LEFT, -1);
	
	SquareConnect(state[10],state[6], UP,   0);

	/* now we have the Squares, we can add a Policy */

	p = Policy(state[0], 0.99, &compute_transition, UP);

	printf("initial state\n");
	SquarePrint(state[0]);

	for(convergence = 0, i = 0; !convergence; i++) {
		printf("-----\nitertation %d\n", i + 1);
		if(!PolicyIterate(p)) convergence = -1;
		SquarePrint(state[0]);
	}

	/* it erases all the squares, too */
	Policy_(&p);

	printf("\nand now for something completely different\n\n");

	/* now we have a different problem:
	 0 5  0
	 5 10 0 */

	for(i = 0; i < N2; i++) {
		reward = 0;
		if(i == 1 || i == 3) reward = 5;
		else if(i == 4) reward = 10;
		state[i] = Square(reward, LEFT);
	}

	SquareConnect(state[0], state[1], LEFT, -1);
	SquareConnect(state[1], state[2], LEFT, -1);

	SquareConnect(state[3], state[4], LEFT, -1);
	SquareConnect(state[4], state[5], LEFT, -1);

	SquareConnect(state[0], state[3], DOWN, -1);
	SquareConnect(state[1], state[4], DOWN, -1);
	SquareConnect(state[2], state[5], DOWN, -1);

	p = Policy(state[0], 0.9, &compute_transition_2, UP);

	printf("initial state\n");
	SquarePrint(state[0]);

	for(convergence = 0, i = 0; !convergence || i < 5; i++) {
		printf("-----\nitertation %d\n", i + 1);
		if(!PolicyIterate(p)) convergence = -1;
		SquarePrint(state[0]);
	}

	/* it erases all the squares, too */
	Policy_(&p);

	return EXIT_SUCCESS;
}

/** compute (meh) */
static double compute_transition(const enum Direction dir, const struct Square *from, const struct Square *to) {
	struct Square *neighbor;
	double t = 0.0;
	int d;

	for(d = 0; d < ACTIONS; d++) {
		neighbor = SquareGetAdjacent(from, d);
		if(!neighbor) neighbor = (struct Square *)from;
		if(neighbor == to) t += (d == dir) ? 0.7 : 0.1;
	}
	/* printf("(%d) %d -> %d: %f\n", dir, SquareGetIndex(from), SquareGetIndex(to), t); */
	return t;
}

/** compute */
static double compute_transition_2(const enum Direction dir, const struct Square *from, const struct Square *to) {
	struct Square *neighbor;
	double t = 0.0;
	int d;
	
	for(d = 0; d < ACTIONS; d++) {
		neighbor = SquareGetAdjacent(from, d);
		if(!neighbor) neighbor = (struct Square *)from;
		if(neighbor == to) t += (d == dir) ? 1.0 : 0.0;
	}
	/* printf("(%d) %d -> %d: %f\n", dir, SquareGetIndex(from), SquareGetIndex(to), t); */
	return t;
}

static void usage(const char *argvz) {
	fprintf(stderr, "Usage: %s\n", argvz);
	fprintf(stderr, "Version %d.%d.\n\n", versionMajor, versionMinor);
	fprintf(stderr, "%s Copyright %s Neil Edelman\n", programme, year);
	fprintf(stderr, "This program comes with ABSOLUTELY NO WARRANTY.\n");
	fprintf(stderr, "This is free software, and you are welcome to redistribute it\n");
	fprintf(stderr, "under certain conditions; see copying.txt.\n\n");
}
