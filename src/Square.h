struct Square;

/*#define N (11)*/

#define ACTIONS (4)

/* should have ACTION cardinality */
enum Direction { ROOT = -2, NOWHERE = -1, UP = 0, RIGHT, DOWN, LEFT };

struct Square *Square(const double value);
void Square_(struct Square **s_ptr);
int SquareCount(struct Square *start);
void SquareNumber(struct Square *start, struct Square *vector[]);
void SquarePrint(struct Square *s);
void SquareConnect(struct Square *s, struct Square * q, const enum Direction dir, int isTwoWay);
int SquareGetIndex(const struct Square *s);
double SquareGetReward(const struct Square *s);
double SquareGetValue(const struct Square *s);
void SquareSetValue(struct Square *s, const double v);
enum Direction SquareGetAction(const struct Square *s);
struct Square *SquareGetAdjacent(const struct Square *s, const enum Direction dir);
