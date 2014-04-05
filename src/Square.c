/* Copyright 2014 Neil Edelman, distributed under the terms of the
 GNU General Public License, see copying.txt */

/** Node object
 @author Neil */

#include <stdlib.h> /* malloc free */
#include <stdio.h>  /* fprintf */
#include <string.h>
#include "Square.h"

/* for printing */
#define N (5)

struct Square {
	int index;
	double reward;
	double value;
	enum Direction action;
	struct Square *adjacent[ACTIONS];
	/* for doing dfs */
	struct Square *parent, *next;
	enum Direction dir;
};

/* private */
enum Direction backwards(const enum Direction dir);
void move(const enum Direction dir, int *x_ptr, int *y_ptr);
int square2index(const struct Square *s, const struct Square *state[]);

/** constructor */
struct Square *Square(const double reward) {
	struct Square *s;
	int i;

	if(!(s = malloc(sizeof(struct Square)))) {
		perror("Square constructor");
		Square_(&s);
		return 0;
	}
	s->reward = reward;
	s->index  = 0;
	s->value  = reward;
	s->action = UP; /* fixme: should have param to calc */
	for(i = 0; i < ACTIONS; i++) s->adjacent[i] = 0;
	s->parent = 0;
	s->next   = 0;
	s->dir    = NOWHERE;
	fprintf(stderr, "Square: new, #%p.\n", (void *)s);

	return s;
}

/** destructor */
void Square_(struct Square **s_ptr) {
	struct Square *s;

	if(!s_ptr || !(s = *s_ptr)) return;
	fprintf(stderr, "~Square: erase, #%p.\n", (void *)s);
	free(s);
	*s_ptr = s = 0;
}

/* counts the squares in a connected subgraph
 @return n the number of squares */
int SquareCount(struct Square *start) {
	struct Square *s = start, *e = start, *next;
	enum Direction dir;
	int n = 0;

	if(!start) return 0;

	/* place the node in the selected set */
	s->parent = 0;
	s->dir = ROOT;
	/* do bfs */
	do {
		n++;
		/* add the un-selected neighbors to the list bfs */
		for(dir = 0; dir < ACTIONS; dir++) {
			if(!(next = s->adjacent[dir]) || (next->dir != NOWHERE)) continue;
			e->next      = next;
			next->parent = s;
			next->dir    = dir;
			e            = next;
		}
		/* move along the queue */
		s = s->next;
	} while(s);

	/* erase */
	for(s = start; s; s = next) {
		next = s->next;
		s->next   = 0;
		s->parent = 0;
		s->dir    = NOWHERE;
	}

	return n;
}

/* orders them (by index) and assigns the array to squares (the size of the
 array should be a recent SquareCount()) */
void SquareNumber(struct Square *start, struct Square *vector[]) {
	struct Square *s = start, *e = start, *next;
	enum Direction dir;
	int n = 0;

	if(!start || !vector) return;

	/* place the node in the selected set */
	s->parent = 0;
	s->dir = ROOT;
	/* do bfs */
	do {
		/* pop */
		vector[n] = s;
		s->index = n++;
		/* add the un-selected neighbors to the list bfs */
		for(dir = 0; dir < ACTIONS; dir++) {
			if(!(next = s->adjacent[dir]) || (next->dir != NOWHERE)) continue;
			e->next      = next;
			next->parent = s;
			next->dir    = dir;
			e            = next;
		}
		/* move along the queue */
		s = s->next;
	} while(s);

	/* erase */
	for(s = start; s; s = next) {
		next = s->next;
		s->next   = 0;
		s->parent = 0;
		s->dir    = NOWHERE;
	}
}

/** print by bfs */
void SquarePrint(struct Square *start) {
	struct Square *s = start, *e = start, *next, *cursor;
	struct Square *print[N + 1 + N][N + 1 + N];
	enum Direction dir;
	int x, y, min_x = N, max_x = N, min_y = N, max_y = N;
	char print_dir;

	if(!start) return;
	
	/* initialise to null */
	for(y = 0; y < N + 1 + N; y++) {
		for(x = 0; x < N + 1 + N; x++) {
			print[y][x] = 0;
		}
	}

	/* place the node in the selected set */
	s->parent = 0;
	s->dir    = ROOT;
	/* do dfs */
	do {
		/* add the un-selected neighbors to the list dfs */
		for(dir = 0; dir < ACTIONS; dir++) {
			if(!(next = s->adjacent[dir]) || (next->dir != NOWHERE)) continue;
			e->next      = next;
			next->parent = s;
			next->dir    = dir;
			e            = next;
		}
		/* place in buffer */
		for(x = N, y = N, cursor = s; cursor; cursor = cursor->parent) {
			move(backwards(cursor->dir), &x, &y);
		}
		if(x > 0 && x <= N + 1 + N && y > 0 && y <= N + 1 + N) print[y][x] = s;
		/* expand window */
		if(x < min_x) min_x = x;
		if(max_x < x) max_x = x;
		if(y < min_y) min_y = y;
		if(max_y < y) max_y = y;
		/* move along the queue */
		s = s->next;
	} while(s);

	/* print */
	for(y = min_y; y <= max_y; y++) {
		for(x = min_x; x <= max_x; x++) {
			/* out of bounds */
			if(y < 0 || y >= N + 1 + N || x < 0 || x >= N + 1 + N) {
				printf(".");
				continue;
			}
			/* get s */
			s = print[y][x];
			/* null */
			if(!s) {
				printf("      --x-- ");
				continue;
			}
			/* nice try, but extended ASCII is not suppoted in my terminal */
			switch(s->action) {
				case UP:    print_dir = /*'\xCA'*/'^'; break;
				case RIGHT: print_dir = /*'\xCC'*/'>'; break;
				case DOWN:  print_dir = /*'\xCB'*/'v'; break;
				case LEFT:  print_dir = /*'\xB9'*/'<'; break;
				default:    print_dir = /*'\xCE'*/'*'; break;
			}
			/* print square */
			printf("(%2d)%+7.2f%c", s->index, s->value, print_dir);
		}
		printf("\n");
	}

	/* erase */
	for(s = start; s; s = next) {
		next = s->next;
		s->next   = 0;
		s->parent = 0;
		s->dir    = NOWHERE;
	}

	/*printf("(%d:%d, %d:%d)\n", min_x, max_x, min_y, max_y);*/
}

void SquareConnect(struct Square *s, struct Square * q, const enum Direction dir, int isTwoWay) {
	const enum Direction back = backwards(dir);

	if(!s || !q) return;

	s->adjacent[dir]  = q;
	if(!isTwoWay) return;
	q->adjacent[back] = s;
}

int SquareGetIndex(const struct Square *s) {
	if(!s) return 0;
	return s->index;
}

double SquareGetReward(const struct Square *s) {
	if(!s) return 0;
	return s->reward;
}

double SquareGetValue(const struct Square *s) {
	if(!s) return 0.0;
	return s->value;
}

void SquareSetValue(struct Square *s, const double v) {
	if(!s) return;
	s->value = v;
}

enum Direction SquareGetAction(const struct Square *s) {
	if(!s) return UP; /* or sth */
	return s->action;
}

struct Square *SquareGetAdjacent(const struct Square *s, const enum Direction dir) {
	if(!s) return 0;
	return s->adjacent[dir];
}

/* private */

/** returns a Directon that is backwards */
enum Direction backwards(const enum Direction dir) {
	switch(dir) {
		case UP:    return DOWN;
		case RIGHT: return LEFT;
		case DOWN:  return UP;
		case LEFT:  return RIGHT;
		case ROOT:
		case NOWHERE:
			break;
	}
	return -1;
}

/** given (x_ptr, y_ptr) (references to (x, y)), moves one unit in dir */
void move(const enum Direction dir, int *x_ptr, int *y_ptr) {
	switch(dir) {
		case UP:    (*y_ptr)++; return;
		case RIGHT: (*x_ptr)++; return;
		case DOWN:  (*y_ptr)--; return;
		case LEFT:  (*x_ptr)--; return;
		case ROOT:
		case NOWHERE:
			break;
	}
}
