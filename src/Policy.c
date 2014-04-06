/* Copyright 2014 Neil Edelman, distributed under the terms of the
 GNU General Public License, see copying.txt */

/** policy iteration
 @author Neil */

#include <stdlib.h> /* malloc free */
#include <stdio.h>  /* fprintf */
#include "Square.h"
#include "Policy.h"

/* arbitrary */
#define EPSILON 5E-5

struct Policy {
	int n;
	struct Square **vector;
	double **transition[ACTIONS];
	double discount;
};

double mult(const double p, const int n, const double *q);

/* public */

/** constructor
 @param start    any Square vertex thats in the connected set
 @param discount discount factor (0, 1)
 @param t        the function to calculate the transition matrix for Direction from Square to Square */
struct Policy *Policy(struct Square *start, const double discount, double (*t)(const enum Direction, const struct Square *, const struct Square *), const enum Direction def) {
	struct Policy *p;
	double *data;
	int d, i, j;
	int n;

	if(!start || discount <= 0.0 || discount >= 1.0 || !t) return 0;

	n = SquareCount(start);

	if(!(p = malloc(sizeof(struct Policy)))) {
		perror("Policy constructor");
		Policy_(&p);
		return 0;
	}
	p->n        = n;
	p->vector   = 0;
	for(i = 0; i < ACTIONS; i++) p->transition[i] = 0;
	p->discount = discount;

	/* allocate more! */
	if(!(p->vector = malloc(sizeof(struct Square *) * n))) {
		perror("Policy constructor");
		Policy_(&p);
		return 0;
	}

	/* number the squares; important that SquareNumber is not called within
	 start connected domain */
	SquareNumber(start, p->vector);

	/* fill out the policy matrices */
	for(d = 0; d < ACTIONS; d++) {
		if(!(p->transition[d] = malloc(sizeof(double *) * n + sizeof(double) * n * n))) {
			perror("Policy constructor");
			Policy_(&p);
			return 0;
		}
		data = (double *)(p->transition[d] + n + 1);
		for(i = 0; i < n; i++) {
			p->transition[d][i] = data + i * n;
			for(j = 0; j < n; j++) {
				/* fixme! I have NO IDEA why JUST THE RIGHT AND THE LEFT are swapped */
				/* LEFT and RIGHT are actually U, R, D, L */
				enum Direction a = d;
				if(a == LEFT) a = RIGHT;
				else if(a == RIGHT) a = LEFT;
				p->transition[d][i][j] = t(a, p->vector[i], p->vector[j]);
			}
		}
	}

	fprintf(stderr, "Policy: new, #%p with %d states.\n", (void *)p, n);
	for(d = 0; d < ACTIONS; d++) {
		for(j = 0; j < n; j++) {
			for(i = 0; i < n; i++) {
				printf("%+5.2f ", p->transition[d][j][i]);
			}
			printf("\n");
		}
		printf("\n");
	}
	
	return p;
}

/** destructor (it deletes all the squares associated to that Policy!) */
void Policy_(struct Policy **p_ptr) {
	struct Policy *p;
	int i;

	if(!p_ptr || !(p = *p_ptr)) return;
	fprintf(stderr, "~Policy: erase, #%p.\n", (void *)p);
	if(p->vector) {
		/* erase the squares, too! (meh, saves having to delete them) */
		for(i = 0; i < p->n; i++) Square_(&p->vector[i]);
		free(p->vector);
	}
	for(i = 0; i < ACTIONS; i++) if(p->transition[i]) free(p->transition[i]);
	free(p);
	*p_ptr = p = 0;
}

/* @return boolean whether you need to do it again */
int PolicyIterate(struct Policy *p) {
	struct Square *s;
	enum Direction a;
	double choose[ACTIONS], choice, next_value, delta;
	int i, j;
	int isSmall = -1;

	if(!p) return 0;
	for(i = 0; i < p->n; i++) {
		s = p->vector[i];
		/*a = SquareGetAction(s);*/
		for(a = 0; a < ACTIONS; a++) {
			for(choose[a] = 0.0, j = 0; j < p->n; j++) {
				/*printf("%f*%f + ", p->transition[a][i][j], SquareGetValue(p->vector[j]));*/
				choose[a] += p->transition[a][i][j] * SquareGetValue(p->vector[j]);
			}
		}
		/* (lol) choose alphabetically */
		a      = DOWN;
		choice = choose[DOWN];
		if(choose[LEFT]  > choice) { a = LEFT;  choice = choose[LEFT];  }
		if(choose[RIGHT] > choice) { a = RIGHT; choice = choose[RIGHT]; }
		if(choose[UP]    > choice) { a = UP;    choice = choose[UP];    }
		SquareSetAction(s, a);
		/*printf("= %f\n", m);*/
		next_value = SquareGetReward(s) + p->discount * choice;
		delta = SquareGetValue(s) - next_value;
		if(isSmall && (delta < -EPSILON || delta > EPSILON)) isSmall = 0;
		SquareSetValue(s, next_value);
		/*printf("iterating node %d: %f = %f + %f*(%f x row%d(%d))\n", i, u, r, p->discount, v, a, x);*/
	}
	return isSmall ? 0 : -1;
}

/* private */

/* not used */
double mult(const double p, const int n, const double *q) {
	double dot = 0;
	int i;

	for(i = 0; i < n; i++) {
		dot += p * q[i];
	}
	return dot;
}
