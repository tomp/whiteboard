
## Eight Queens Puzzle

https://en.wikipedia.org/wiki/Eight_queens_puzzle

The N-Queens puzzle refers to a set of related questions that stem from the challenge of arranging eight queens on a standard 8x8 chess board in such a way that no two queeens are attacking one another.  The more general problem extends this to N queens on an NxN chess board.  Except for N=2 and N=3, this is know to have solutions for all positive N.

Some of the different forms of the challenge are:
1. Find all of the arrangements of queens that satisfy the condition.
2. Just count all of the arrangements of queens that satisfy the condition.
3. Find at least *one* arrangment of queens that satify the condition.

The second challenge (counting) has been solved up to N=27. (The number of solutions grows exponentially with N.)

The third challenge (finding at least one arrangement) can solved for much, much larger values of N.  (Solutions up to N=500,000 have been reported.)

**Utility functions**


```python
import timeit

def timings(fn, maxtime=15, runs=0):
    """Time the given function, fn(N), for increasing values of N, starting from N=6.
    This continues until the calculation time exceeds `maxtime` seconds.
    If a non-zero value for `runs` is given, the function is evaluted that many times
    for each value of N.  If `runs` is zero, we try to pick a number of evaluations
    that totals to roughly 1 second.
    
    A list of (n, timing) tuples is returned.
    """
    result = []
    n, exec_time = 6, 0.001
    while exec_time < maxtime:
        nrun = runs or max(1, int(0.2/exec_time))  # assume time increases ~5x each round.
        exec_time = timeit.timeit("fn(n)", number=nrun, globals=locals()) / nrun
        result.append((n, exec_time))
        n += 1              
    return result
```


```python
import numpy as np

def expfit(n, y):
    """Fit the (n, y) data to a simple exponential model y = exp(a*x + b) and print the results.
    The factor exp(a) and offset b are displayed, along with the predictions for each point.
    
    A list of (n, timing, predicted timing) tuples is also returned.
    """
    cf = np.polyfit(n, np.log(y), 1)
    print("exp(a): {:3f}\nb: {}".format(np.exp(cf[0]), cf[1]))
    yp = np.exp(np.polyval(cf, n))
    print("\n".join(["{:2d}: {:6f}  pred: {:6f}".format(*val) for val in zip(n, y, yp)]))
    return list(zip(n, y, yp))
```


```python
from bokeh.plotting import figure, output_notebook, show
from bokeh.palettes import Category20 
output_notebook()

def plot_timings(*named_timings, log=True):
    """Plot the given timings, each a tuple of the function name, and a matrix of timing data.
    Each matrix row is a 
    """
    args = {'title': "Timings",
            'x_axis_label': 'N',
            'y_axis_label': 'time (sec)'}
    if log:
        args['y_axis_type'] = 'log'

    fig = figure(**args)
    
    # add a line renderer with legend and line thickness
    for i, timing in enumerate(named_timings):
        name, xyp = timing
        if len(xyp[0]) == 3:
            n, y, yp = zip(*xyp)
            fig.line(n, y, legend=name, line_width=2, color=Category20[20][2*i])
            fig.line(n, yp, legend=name + " (pred)", line_width=1, color=Category20[20][2*i+1])
        elif len(xyp[0]) == 2:
            n, y = zip(*xyp)
            fig.line(n, y, legend=name, line_width=2, color=Category20[20][2*i])
        else:
            raise ValueError("Can't decipher timing")

    # show the results
    show(fig)
```



    <div class="bk-root">
        <a href="https://bokeh.pydata.org" target="_blank" class="bk-logo bk-logo-small bk-logo-notebook"></a>
        <span id="92509c89-d69f-4178-ad28-4036e9878ab3">Loading BokehJS ...</span>
    </div>





```python
def print_solutions(solns):
    for i, b in enumerate(solns):
        print("{:2d}: {}".format(i+1, " ".join(map(str, b))))
```

### 1. Consider all possible arrangements of the N queens on distinct rows and columns.
* This is the most reasonable "brute-force" solution to the problem, because we know that, for every solution, the column numbers of the queens will be some permution of the integers 1 to N.  We can generate those N! permutations directly.


```python
NQUEEN = 8
```


```python
from itertools import permutations

def attack(b):
    """Return True if any two queens on the given board attack each other."""
    for i in range(len(b)-1):
        for j in range(i+1, len(b)):
            if abs(b[i] - b[j]) == j - i:
                return True
    return False
    
def solution1(nqueen):
    solns = []
    for board in permutations(list(range(nqueen))):
        if not attack(board):
            solns.append(tuple(board))
    return solns

solns = solution1(NQUEEN)
print("Found {} solutions".format(len(solns)))
# print_solutions(solns)
```

    Found 92 solutions



```python
time1 = timings(solution1)
n, y = zip(*time1)
pred1 = expfit(n, y)
```

    exp(a): 9.351711
    b: -19.859046516005407
     6: 0.001941  pred: 0.001587
     7: 0.014305  pred: 0.014844
     8: 0.114603  pred: 0.138821
     9: 1.140848  pred: 1.298212
    10: 11.839864  pred: 12.140502
    11: 136.153895  pred: 113.534471


### 2. Find solutions using an exhaustive depth-first search.
* Doing a classic tree search for solutions should let us reduce the number of positions we need to look at, significantly, because it lets us backtrack as soon as we find that some queen placement prevents further queens from being placed.
* In principle, a breadth-first search wouldn't examine more positions than this depth-first search, but it would require storing many more intermediate states.  Since we want to find all solutions, anyway, a depth-first search makes more sense.

#### a. Reuse the *attack()* function from the brute-force search


```python
def solution2(nqueen):
    solns = []
    queue = [[i] for i in range(nqueen)]
    while queue:
        b = queue.pop()
        for j in range(nqueen):
            if not(j in b or attack(b + [j])):
                newb = b + [j]
                if len(newb) == nqueen:
                    solns.append(newb)
                else:
                    queue.append(newb)
    return solns
              
solns = solution2(NQUEEN)
print("Found {} solutions".format(len(solns)))
```

    Found 92 solutions



```python
time2 = timings(solution2)
n, y = zip(*time2)
pred2 = expfit(n, y)
```

    exp(a): 5.267663
    b: -16.574365515359947
     6: 0.001537  pred: 0.001354
     7: 0.007168  pred: 0.007131
     8: 0.035930  pred: 0.037565
     9: 0.178009  pred: 0.197882
    10: 0.908257  pred: 1.042376
    11: 5.407617  pred: 5.490888
    12: 34.333706  pred: 28.924150


#### b. Only check attacks on newly added queens 


```python
def adds_attack(b, row):
    """Return True if adding a queen in the next column on given row results in an attack.
    This should be all we need to check, if we know the input board b contains no attacks.
    """
    col = len(b)
    return any([abs(irow - row) == col - icol
                for icol, irow in enumerate(b)])

def solution2b(nqueen):
    solns = []
    queue = [[i] for i in range(nqueen)]
    while queue:
        b = queue.pop()
        for j in range(nqueen):
            if not(j in b or adds_attack(b, j)):
                newb = b + [j]
                if len(newb) == nqueen:
                    solns.append(newb)
                else:
                    queue.append(newb)
    return solns
              
solns = solution2b(NQUEEN)
print("Found {} solutions".format(len(solns)))
```

    Found 92 solutions



```python
time2b = timings(solution2b)
n, y = zip(*time2b)
pred2b = expfit(n, y)
```

    exp(a): 4.900585
    b: -16.74233765946474
     6: 0.000934  pred: 0.000742
     7: 0.003853  pred: 0.003636
     8: 0.015886  pred: 0.017819
     9: 0.075273  pred: 0.087323
    10: 0.351358  pred: 0.427932
    11: 1.874776  pred: 2.097116
    12: 10.534047  pred: 10.277098
    13: 65.299843  pred: 50.363793


### 3. Track search state using tuples of bit vectors.
The state of a partially completed board is represented by separate bit vectors that record the queen locations, right-diagonal attacks, left-diagonal attacks, and a list with the location of each queen.


```python
from collections import namedtuple
from itertools import islice

def bits(i, nbits=NQUEEN):
    """Return a string containing the nbits-binary representation of the integer i."""
    return bin((1 << nbits) | i)[3:]

Queens = namedtuple('Queens', ['col', 'rd', 'ld', 'loc'])
Queens.__repr__ = lambda q: "{}, {}, {} ({})".format(bits(q.col), bits(q.rd), bits(q.ld), ",".join(map(str, q.loc)))

def successors(state, nqueen):
    col = nqueen - 1
    newq = 1 << col
    excl = state.col | state.ld | state.rd
    while newq:
        if not (newq & excl):
            yield Queens(state.col | newq,(state.rd | newq) >> 1, (state.ld | newq) << 1, state.loc + (col,))
        col -= 1
        newq = newq >> 1
```


```python
# Verify that successors() works the way we expect it to.
q0 = Queens(0, 0, 0, ())
print(q0)
print('- - - - - -')
for q in successors(q0, NQUEEN):
    print(q)
print('===========')
q1 = list(successors(q0, NQUEEN))[3]
print(q1)
print('- - - - - -')
for q in successors(q1, NQUEEN):
    print(q)
print('===========')
q2 = list(successors(q1, NQUEEN))[0]
print(q2)
print('- - - - - -')
for q in successors(q2, NQUEEN):
    print(q)
```

    00000000, 00000000, 00000000 ()
    - - - - - -
    10000000, 01000000, 00000000 (7)
    01000000, 00100000, 10000000 (6)
    00100000, 00010000, 01000000 (5)
    00010000, 00001000, 00100000 (4)
    00001000, 00000100, 00010000 (3)
    00000100, 00000010, 00001000 (2)
    00000010, 00000001, 00000100 (1)
    00000001, 00000000, 00000010 (0)
    ===========
    00010000, 00001000, 00100000 (4)
    - - - - - -
    10010000, 01000100, 01000000 (4,7)
    01010000, 00100100, 11000000 (4,6)
    00010100, 00000110, 01001000 (4,2)
    00010010, 00000101, 01000100 (4,1)
    00010001, 00000100, 01000010 (4,0)
    ===========
    10010000, 01000100, 01000000 (4,7)
    - - - - - -
    10110000, 00110010, 111000000 (4,7,5)
    10011000, 00100110, 110010000 (4,7,3)
    10010010, 00100011, 110000100 (4,7,1)
    10010001, 00100010, 110000010 (4,7,0)


#### a. Find all solutions


```python
def solution3(nqueen):
    solns = []
    complete = (1 << nqueen) - 1
    queue = [q for q in successors(Queens(0, 0, 0, ()), nqueen)]
    while queue:
        q = queue.pop()       
        if q.col == complete:
            solns.append(q.loc)
        else:
            for newq in successors(q, nqueen):
                queue.append(newq)
    return solns
              
solns = solution3(NQUEEN)
print("Found {} solutions".format(len(solns)))
```

    Found 92 solutions



```python
time3 = timings(solution3)
n, y = zip(*time3)
pred3 = expfit(n, y)
```

    exp(a): 4.470174
    b: -16.58158031509165
     6: 0.000690  pred: 0.000502
     7: 0.002276  pred: 0.002244
     8: 0.008711  pred: 0.010030
     9: 0.036968  pred: 0.044836
    10: 0.163304  pred: 0.200427
    11: 0.780103  pred: 0.895943
    12: 4.251360  pred: 4.005022
    13: 23.824188  pred: 17.903145


#### b. Just count the number of solutions
* Keeping track of which queens are in which columns isn't necessary if you just want to count the number of solutions.  So, we can simplify things a little by not keeping track of that.
* The speedup from this is much less than I expected. (~8%)


```python
Queens2 = namedtuple('Queens', ['col', 'rd', 'ld'])
Queens2.__repr__ = lambda q: "{}, {}, {}".format(bits(q.col), bits(q.rd), bits(q.ld))

def successors2(state, nqueen):
    col = nqueen - 1
    newq = 1 << col
    excl = state.col | state.ld | state.rd
    while newq:
        if not (newq & excl):
            yield Queens2(state.col | newq, (state.rd | newq) >> 1, (state.ld | newq) << 1)
        col -= 1
        newq = newq >> 1
        
def solution3b(nqueen):
    count = 0
    complete = (1 << nqueen) - 1
    queue = [q for q in successors2(Queens2(0, 0, 0), nqueen)]
    while queue:
        q = queue.pop()       
        if q.col == complete:
            count += 1
        else:
            for newq in successors2(q, nqueen):
                queue.append(newq)
    return count
              
nsolns = solution3b(NQUEEN)
print("Found {} solutions".format(nsolns))
```

    Found 92 solutions



```python
time3b = timings(solution3b)
n, y = zip(*time3b)
pred3b = expfit(n, y)
```

    exp(a): 4.484820
    b: -16.6794474460244
     6: 0.000571  pred: 0.000464
     7: 0.002461  pred: 0.002082
     8: 0.008115  pred: 0.009336
     9: 0.033822  pred: 0.041871
    10: 0.147008  pred: 0.187784
    11: 0.754938  pred: 0.842177
    12: 3.996532  pred: 3.777013
    13: 22.350366  pred: 16.939223


#### c. Replace namedtuples with simple tuples
* I was just curious how much overhead there was in using namedtuples.  It turns out it's significant. (~2x speedup)


```python
def successors3(col, rd, ld, nqueen):
    newq = 1 << (nqueen - 1)
    excl = col | rd | ld
    while newq:
        if not (newq & excl):
            yield (col | newq, (rd | newq) >> 1, (ld | newq) << 1)
        newq = newq >> 1
        
def solution3c(nqueen):
    count = 0
    done = (1 << nqueen) - 1
    queue = list(successors3(0, 0, 0, nqueen))
    while queue:
        col, rd, ld = queue.pop()       
        if col == done:
            count += 1
        else:
            queue.extend(successors3(col, rd, ld, nqueen))
    return count
              
nsolns = solution3c(NQUEEN)
print("Found {} solutions".format(nsolns))
```

    Found 92 solutions



```python
time3c = timings(solution3c)
n, y = zip(*time3c)
pred3c = expfit(n, y)
```

    exp(a): 4.670683
    b: -17.50889589973632
     6: 0.000372  pred: 0.000258
     7: 0.001244  pred: 0.001207
     8: 0.005280  pred: 0.005637
     9: 0.021300  pred: 0.026327
    10: 0.101429  pred: 0.122967
    11: 0.437754  pred: 0.574338
    12: 2.445329  pred: 2.682552
    13: 13.654391  pred: 12.529350
    14: 83.227663  pred: 58.520626


### 4. Replace iterative search with a recursive search
* This doesn't reduce the number of positions that are examined, but it avoids having to manage a queue.  This both simplifies the logic a little, and reduces memory use.
* The logic for deciding when to give up is a little more efficient here, too.


```python
def solution4(nqueen):
    done = (1 << nqueen) - 1

    def count4(level, col, rd, ld):
        excl = (col | rd | ld) & done
        if excl == done:
            return 0
        elif level+1 == nqueen:
            return 1
        
        count = 0
        newq = 1 << (nqueen - 1)
        while newq:
            if (newq & excl) == 0:
                    count += count4(level+1, col | newq, (rd | newq) >> 1, (ld | newq) << 1)
            newq = newq >> 1
        return count
        
    return count4(0, 0, 0, 0)

              
nsolns = solution4(NQUEEN)
print("Found {} solutions".format(nsolns))
```

    Found 92 solutions



```python
time4 = timings(solution4)
n, y = zip(*time4)
pred4 = expfit(n, y)
```

    exp(a): 4.671884
    b: -17.89365250898106
     6: 0.000234  pred: 0.000176
     7: 0.000932  pred: 0.000823
     8: 0.003743  pred: 0.003844
     9: 0.013693  pred: 0.017960
    10: 0.070447  pred: 0.083909
    11: 0.305075  pred: 0.392011
    12: 1.580122  pred: 1.831430
    13: 9.564014  pred: 8.556227
    14: 56.738100  pred: 39.973701


### Plot timings for the different solutions


```python
plot_timings(('soln1', pred1), ('soln2', pred2), ('soln2b', pred2b), ('soln3', pred3), ('soln3c', pred3c), ('soln4', pred4))
```



<div class="bk-root">
    <div class="bk-plotdiv" id="b7f2e9fa-c6f0-46a7-9816-e69e82de3be4"></div>
</div>



