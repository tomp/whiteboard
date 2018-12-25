#include <stdio.h>
#include <string.h>

#define MAXSIZE 32
#define NQUEEN 14

typedef struct state {
    u_int32_t col;  // columns containing queens
    u_int32_t ld;   // columns under left-diagonal attack
    u_int32_t rd;   // columns under right-diagonal attack
} stateType;

void state_print(stateType *s, int nbits);
void binstr(u_int32_t value, int nbits, char *buf, int buflen);

const u_int32_t done = (1 << NQUEEN) - 1;
stateType board[MAXSIZE];

int main(int argc, char** argv)
{
    int count;

    printf("Count solutions to the %d-queens problem\n", NQUEEN);

    stateType initState = {0, 0, 0};
    memcpy(&board[0], &initState, sizeof(struct state));
    // state_print(&board[0], NQUEEN);

    count = count_solutions(0);
    printf("Found %d solutions\n", count);

    return 0;
}

void state_print(stateType *s, int nbits)
{
    char col_buf[MAXSIZE];
    char ld_buf[MAXSIZE];
    char rd_buf[MAXSIZE];

    binstr(s->col, nbits, &col_buf, MAXSIZE);
    binstr(s->ld, nbits, &ld_buf, MAXSIZE);
    binstr(s->rd, nbits, &rd_buf, MAXSIZE);

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
    buf[nbits] = NULL;
}

int count_solutions(int level)
{
    u_int32_t newq = 1 << NQUEEN;
    u_int32_t excl = board[level].col | board[level].ld | board[level].rd;
    int solutions = 0;

    // printf("count_solutions(%d):  ", level);
    // state_print(&board[level], NQUEEN);
    // printf("\n");

    for (newq = 1 << NQUEEN; newq; newq = newq >> 1) {
        if (!(excl & newq)) {
            if ((board[level].col | newq) == done) {
                // printf("SOLUTION\n");
                return 1;
            }
            board[level + 1].col = board[level].col | newq;
            board[level + 1].ld = (board[level].ld | newq) << 1;
            board[level + 1].rd = (board[level].rd | newq) >> 1;
            solutions += count_solutions(level+1);
        }
    }
    return solutions;
}
