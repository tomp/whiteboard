
## Eight Queens Puzzle

**Task: Count the number of ways you can place N queens on an NxN chess board, such that no two queens are attacking one another.**

**Utility functions**


```python
import numpy as np
    
def print_solutions(solns):
    for i, b in enumerate(solns):
        print("{:2d}: {}".format(i+1, " ".join(map(str, b))))

def timings(fn, maxtime=15, nrun=1):
    result = []
    n, dt = 6, 0
    while dt < maxtime:
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
        <span id="948e2222-71d8-4505-b918-8d9c5050a689">Loading BokehJS ...</span>
    </div>




### 1. Consider all possible arrangements of the 8 queens on distinct rows and columns.


```python
NQUEEN = 10
```


```python
import timeit
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

    exp(a): 9.149036
    b: -19.683358688872186
     6: 0.002185  pred: 0.001659
     7: 0.013701  pred: 0.015179
     8: 0.111955  pred: 0.138876
     9: 1.089616  pred: 1.270585
    10: 11.465833  pred: 11.624630
    11: 131.192340  pred: 106.354154


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

    exp(a): 5.278673
    b: -16.625263718561403
     6: 0.001448  pred: 0.001303
     7: 0.007182  pred: 0.006877
     8: 0.033885  pred: 0.036303
     9: 0.170547  pred: 0.191630
    10: 0.911772  pred: 1.011550
    11: 5.250576  pred: 5.339643
    12: 32.986274  pred: 28.186233


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

    exp(a): 4.905262
    b: -16.721051843460007
     6: 0.000896  pred: 0.000762
     7: 0.003798  pred: 0.003739
     8: 0.016835  pred: 0.018342
     9: 0.084330  pred: 0.089970
    10: 0.406006  pred: 0.441328
    11: 1.876752  pred: 2.164829
    12: 10.625082  pred: 10.619054
    13: 63.569585  pred: 52.089241


### 3. Track search state using tuples of bit vectors.
The state of a partially completed board is represented by separate bit vectors that record the queen locations, right-diagonal attacks, left-diagonal attacks, and a list with the location of each queen.


```python
from collections import namedtuple
from itertools import islice

def bits(i, nbits=NQUEEN):
    return bin((1 << nbits) | i)[3:]

Queens = namedtuple('Queens', ['col', 'rd', 'ld', 'loc'])
Queens.__repr__ = lambda q: "{}, {}, {} ({})".format(bits(q.col), bits(q.rd), bits(q.ld), ",".join(map(str, q.loc)))

def new_queens(col):
    newq = 1 << col
    return Queens(newq, newq >> 1, newq << 1, (col,))

def successors(state, nqueen):
    col = nqueen - 1
    newq = 1 << col
    excl = state.col | state.ld | state.rd
    while newq:
        if not (newq & excl):
            yield Queens(state.col | newq, (state.rd | newq) >> 1, (state.ld | newq) << 1, state.loc + (col,))
        col -= 1
        newq = newq >> 1
        
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
    complete = pow(2, nqueen) - 1
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

    exp(a): 4.492428
    b: -16.631245959097228
     6: 0.000649  pred: 0.000492
     7: 0.002254  pred: 0.002211
     8: 0.008735  pred: 0.009931
     9: 0.038000  pred: 0.044614
    10: 0.166367  pred: 0.200425
    11: 0.794567  pred: 0.900397
    12: 4.221937  pred: 4.044969
    13: 23.607198  pred: 18.171731


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
    complete = pow(2, nqueen) - 1
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
expfit(n, y)
```

    exp(a): 4.517612
    b: -16.761267086815444
     6: 0.000576  pred: 0.000447
     7: 0.001985  pred: 0.002019
     8: 0.008686  pred: 0.009119
     9: 0.036272  pred: 0.041196
    10: 0.148847  pred: 0.186107
    11: 0.722061  pred: 0.840761
    12: 4.014271  pred: 3.798233
    13: 22.249151  pred: 17.158946





    [(6, 0.0005755180027335882, 0.0004468151814363395),
     (7, 0.0019850459648296237, 0.0020185377464546103),
     (8, 0.008686258108355105, 0.009118970892537997),
     (9, 0.03627238096669316, 0.04119597480156664),
     (10, 0.14884748705662787, 0.18610744127279155),
     (11, 0.7220614129910246, 0.8407612603886834),
     (12, 4.014271337073296, 3.7982333867792075),
     (13, 22.24915105907712, 17.15894575562967)]



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

    exp(a): 4.712238
    b: -17.563875686708332
     6: 0.000354  pred: 0.000258
     7: 0.001322  pred: 0.001215
     8: 0.005022  pred: 0.005727
     9: 0.021716  pred: 0.026987
    10: 0.101157  pred: 0.127168
    11: 0.580952  pred: 0.599246
    12: 2.452625  pred: 2.823788
    13: 13.912637  pred: 13.306360
    14: 85.006437  pred: 62.702731


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

    exp(a): 4.721219
    b: -18.069168028834724
     6: 0.000226  pred: 0.000157
     7: 0.000773  pred: 0.000743
     8: 0.003179  pred: 0.003508
     9: 0.014298  pred: 0.016563
    10: 0.060618  pred: 0.078199
    11: 0.291841  pred: 0.369195
    12: 1.568045  pred: 1.743048
    13: 9.019769  pred: 8.229313
    14: 55.034934  pred: 38.852386


### Plot timings for the different solutions


```python
plot_timings(('soln1', pred1), ('soln2', pred2), ('soln2b', pred2b), ('soln3', pred3), ('soln3c', pred3c), ('soln4', pred4))
```



<div class="bk-root">
    <div class="bk-plotdiv" id="c626b78d-3f69-4f3d-9258-b493fec77ed7"></div>
</div>



