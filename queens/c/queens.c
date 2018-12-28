#include <stdio.h>
#include <string.h>

#define MAXSIZE 32 // The maximum supported problem size.
#define NQUEEN 14  // The size of the problem.

/**
 * A stateType represents a partially filled NxN chess board in the N-queens
 * problem.  The board is filled progressively from the top row to the bottom.
 * Each row can contain a single queen.
 * The .col field records the columns in which queens have been placed.
 * The .rd and .ld fields record the columns in the next unfilled row that
 * are under diagonal attack by one of the previously placed queens.
 */ 
typedef struct state {
    u_int32_t col;  // columns containing queens
    u_int32_t ld;   // columns under left-diagonal attack
    u_int32_t rd;   // columns under right-diagonal attack
} stateType;

void state_print(stateType *s, int nbits);
void binstr(u_int32_t value, int nbits, char *buf, int buflen);
int count_solutions(stateType s);

// The value of a stateType's .col field when a solution has been found.
const u_int32_t done = (1 << NQUEEN) - 1;

int main(int argc, char** argv)
{
    int count;

    printf("Count solutions to the %d-queens problem\n", NQUEEN);

    stateType initState = {0, 0, 0};
    // state_print(&initState, NQUEEN);

    count = count_solutions(initState);
    printf("Found %d solutions\n", count);

    return 0;
}

/**
 * Print a representation of the given partial board state, s.
 */
void state_print(stateType *s, int nbits)
{
    char col_buf[MAXSIZE];
    char ld_buf[MAXSIZE];
    char rd_buf[MAXSIZE];

    binstr(s->col, nbits, (char *)&col_buf, MAXSIZE);
    binstr(s->ld, nbits, (char *)&ld_buf, MAXSIZE);
    binstr(s->rd, nbits, (char *)&rd_buf, MAXSIZE);

    printf("<col: %s  ld: %s  rd: %s>\n", col_buf, ld_buf, rd_buf);
}

/***
 * Produce an nbits-wide binary representation of the given integer value.
 * It is created in the provided buffer.  If the buffer is too small,
 * the representation is truncated on the left.
 */
void binstr(u_int32_t value, int nbits, char *buf, int buflen)
{
    int i;
    u_int32_t mask;
    
    if (buflen < (nbits + 1))
        mask = 1 << (buflen - 1);
    else
        mask = 1 << nbits;

    for (i=0; i < nbits; i++) {
        if (mask & value)
            buf[i] = '1';
        else
            buf[i] = '0';
        mask = mask >> 1;
    }
    buf[nbits] = 0;
}

/**
 * Recursively count the number of N-queens solutions that can be found from the
 * given partial board state, s.
 */
int count_solutions(const stateType s)
{
    u_int32_t excl = s.col | s.ld | s.rd;
    u_int32_t newq;
    stateType newState;
    int solutions = 0;

    for (newq = 1 << (NQUEEN - 1); newq; newq = newq >> 1) {
        if (!(excl & newq)) {
            if ((s.col | newq) == done) {
                // printf("SOLUTION\n");
                return 1;
            }
            newState.col = s.col | newq;
            newState.ld = (s.ld | newq) << 1;
            newState.rd = (s.rd | newq) >> 1;
            solutions += count_solutions(newState);
        }
    }
    return solutions;
}
