struct Policy;

struct Policy *Policy(struct Square *start, const double discount, double (*t)(const enum Direction, const struct Square *, const struct Square *));
void Policy_(struct Policy **p_ptr);
int PolicyIterate(struct Policy *p);
