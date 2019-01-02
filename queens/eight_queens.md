
## Eight Queens Puzzle

**Task: Count the number of ways you can place N queens on an NxN chess board, such that no two queens are attacking one another.**

**Utility functions**


```python
import numpy as np
import timeit
    
def print_solutions(solns):
    for i, b in enumerate(solns):
        print("{:2d}: {}".format(i+1, " ".join(map(str, b))))

def timings(fn, maxtime=15, nrun=0):
    result = []
    n, dt = 6, 0.001
    while dt < maxtime:
        nrun = max(1, int(0.2/dt))
        dt = timeit.timeit("fn(n)", number=nrun, globals=locals()) / nrun
        result.append((n, dt))
        n += 1              
    return result

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
        <span id="76c5978e-eb05-4d8b-adb9-f57811cca537">Loading BokehJS ...</span>
    </div>




### 1. Consider all possible arrangements of the N queens on distinct rows and columns.


```python
NQUEEN = 10
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

    Found 724 solutions



```python
time1 = timings(solution1)
n, y = zip(*time1)
pred1 = expfit(n, y)
```

    exp(a): 9.521513
    b: -20.04513039053966
     6: 0.001820  pred: 0.001468
     7: 0.013404  pred: 0.013978
     8: 0.110075  pred: 0.133094
     9: 1.078596  pred: 1.267255
    10: 11.899116  pred: 12.066183
    11: 139.236723  pred: 114.888314


### 2. Find solutions using an exhaustive depth-first search.

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

    Found 724 solutions



```python
time2 = timings(solution2)
n, y = zip(*time2)
pred2 = expfit(n, y)
```

    exp(a): 5.243480
    b: -16.55145491075078
     6: 0.001526  pred: 0.001347
     7: 0.007787  pred: 0.007065
     8: 0.032386  pred: 0.037047
     9: 0.163617  pred: 0.194254
    10: 0.930854  pred: 1.018569
    11: 5.345769  pred: 5.340844
    12: 33.316671  pred: 28.004604


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

    Found 724 solutions



```python
time2b = timings(solution2b)
n, y = zip(*time2b)
pred2b = expfit(n, y)
```

    exp(a): 4.854287
    b: -16.648432405440953
     6: 0.001088  pred: 0.000770
     7: 0.003651  pred: 0.003737
     8: 0.015542  pred: 0.018142
     9: 0.075160  pred: 0.088066
    10: 0.339158  pred: 0.427496
    11: 1.858727  pred: 2.075189
    12: 10.469381  pred: 10.073561
    13: 65.571665  pred: 48.899954


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

    0000000000, 0000000000, 0000000000 ()
    - - - - - -
    1000000000, 0100000000, 0000000000 (9)
    0100000000, 0010000000, 1000000000 (8)
    0010000000, 0001000000, 0100000000 (7)
    0001000000, 0000100000, 0010000000 (6)
    0000100000, 0000010000, 0001000000 (5)
    0000010000, 0000001000, 0000100000 (4)
    0000001000, 0000000100, 0000010000 (3)
    0000000100, 0000000010, 0000001000 (2)
    0000000010, 0000000001, 0000000100 (1)
    0000000001, 0000000000, 0000000010 (0)
    ===========
    0001000000, 0000100000, 0010000000 (6)
    - - - - - -
    1001000000, 0100010000, 0100000000 (6,9)
    0101000000, 0010010000, 1100000000 (6,8)
    0001010000, 0000011000, 0100100000 (6,4)
    0001001000, 0000010100, 0100010000 (6,3)
    0001000100, 0000010010, 0100001000 (6,2)
    0001000010, 0000010001, 0100000100 (6,1)
    0001000001, 0000010000, 0100000010 (6,0)
    ===========
    1001000000, 0100010000, 0100000000 (6,9)
    - - - - - -
    1011000000, 0011001000, 11100000000 (6,9,7)
    1001100000, 0010011000, 11001000000 (6,9,5)
    1001001000, 0010001100, 11000010000 (6,9,3)
    1001000100, 0010001010, 11000001000 (6,9,2)
    1001000010, 0010001001, 11000000100 (6,9,1)
    1001000001, 0010001000, 11000000010 (6,9,0)


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

    Found 724 solutions



```python
time3 = timings(solution3)
n, y = zip(*time3)
pred3 = expfit(n, y)
```

    exp(a): 4.472685
    b: -16.57373394920169
     6: 0.000669  pred: 0.000508
     7: 0.002338  pred: 0.002270
     8: 0.008912  pred: 0.010155
     9: 0.038631  pred: 0.045419
    10: 0.168129  pred: 0.203143
    11: 0.789090  pred: 0.908594
    12: 4.215943  pred: 4.063855
    13: 24.044861  pred: 18.176343


#### b. Just count the number of solutions


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

    Found 724 solutions



```python
time3b = timings(solution3b)
n, y = zip(*time3b)
pred3b = expfit(n, y)
```

    exp(a): 4.470358
    b: -16.598125842041316
     6: 0.000649  pred: 0.000494
     7: 0.002319  pred: 0.002208
     8: 0.008206  pred: 0.009869
     9: 0.039181  pred: 0.044117
    10: 0.158368  pred: 0.197219
    11: 0.786077  pred: 0.881639
    12: 4.245627  pred: 3.941242
    13: 22.398824  pred: 17.618762


#### c. Replace namedtuples with simple tuples


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

    Found 724 solutions



```python
time3c = timings(solution3c)
n, y = zip(*time3c)
pred3c = expfit(n, y)
```

    exp(a): 4.654886
    b: -17.44134934692532
     6: 0.000386  pred: 0.000271
     7: 0.001397  pred: 0.001261
     8: 0.005034  pred: 0.005869
     9: 0.024744  pred: 0.027321
    10: 0.091425  pred: 0.127177
    11: 0.477020  pred: 0.591995
    12: 2.436652  pred: 2.755668
    13: 13.975180  pred: 12.827320
    14: 87.310678  pred: 59.709713


### 4. Replace iterative search with a recursive search


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

    Found 724 solutions



```python
time4 = timings(solution4)
n, y = zip(*time4)
pred4 = expfit(n, y)
```

    exp(a): 4.749389
    b: -18.10717767707088
     6: 0.000224  pred: 0.000157
     7: 0.000786  pred: 0.000746
     8: 0.003206  pred: 0.003542
     9: 0.013826  pred: 0.016823
    10: 0.062648  pred: 0.079897
    11: 0.317552  pred: 0.379461
    12: 1.623624  pred: 1.802209
    13: 9.273949  pred: 8.559392
    14: 56.707587  pred: 40.651879


### Plot timings for the different solutions


```python
plot_timings(('soln1', pred1), ('soln2', pred2), ('soln2b', pred2b), ('soln3', pred3), ('soln3c', pred3c), ('soln4', pred4))
```



<div class="bk-root">
    <div class="bk-plotdiv" id="17750240-daee-4c0a-8ec9-d70e47b60487"></div>
</div>



