#include <stdio.h>     // sprintf(), printf()
#include <string.h>    // strcmp()
#include <stdlib.h>    // atoi()

void binstr(u_int32_t value, int nbits, char *buf, int buflen);
int count_solutions(int, u_int32_t, u_int32_t, u_int32_t);
int parse_args(int argc, char** argv);

int nqueen = 8;      // the board size
u_int32_t done = 0;  // The value of 'col' when a solution has been found.

int main(int argc, char** argv)
{
    int count;
    int err;

    err = parse_args(argc, argv);
    if (err) return 1;

    done = (1 << nqueen) - 1;
    printf("Count solutions to the %d-queens problem\n", nqueen);

    count = count_solutions(0, 0, 0, 0);
    printf("Found %d solutions\n", count);

    return 0;
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
 * given partial board state.
 * 
 * A partial board state represents a partially filled NxN chess board in the N-queens
 * problem.  The board is filled progressively from the top row to the bottom.
 * Each row can contain a single queen.  The state variables are: 
 * 
 *   level . the number of queens already placed
 *   col ... bit mask representing columns containing queens
 *   ld .... bit mask representing columns under left-diagonal attack
 *   rd .... bit mask representing columns under right-diagonal attack
 */
int count_solutions(const int level, const u_int32_t col, const u_int32_t ld, const u_int32_t rd)
{
    u_int32_t excl = (col | ld | rd) & done;
    u_int32_t newq;
    int solutions = 0;

    if (excl == done)
        return 0;
    else if (level == nqueen-1)
        return 1;

    for (newq = 1 << (nqueen - 1); newq; newq = newq >> 1) {
        if (!(excl & newq)) {
            solutions += count_solutions(level+1, (col | newq),
                                         (ld | newq) << 1, (rd | newq) >> 1);
        }
    }
    return solutions;
}

int parse_args(int argc, char** argv)
{
    int i = 1;
    char *opt;
    
    while (i < argc) {
        opt = argv[i];
        if (!strcmp(opt, "-n")) {
                nqueen = atoi(argv[i+1]);
                i += 2;
        }
        else {
                printf("Unrecognized argument: '%s'", opt);
                return 1;
        }
    }
    return 0;
}
