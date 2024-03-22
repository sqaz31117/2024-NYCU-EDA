# Lab1 Introduction
1. To exercise the concept of binary decision diagram. 
2. To understand the ordering effects of BDD. 
3. Construct BDDs with given variable orderings, and find the minimum  number of nodes required from the given variable orderings. 

## Package
* [CUDD](<https://gist.github.com/ytakata69/ddfae0e2c1ebc7cd92fc2805dedf3037>)

## Input Format
```
Boolean equation
Variable ordering 1
Variable ordering 2
...
Variable ordering n
```

> Input example
```
ab+cd.
acbd.   // a->c->b->d
abcd.   // a->b->c->d
```

## Output Format
```
6   // Minimum number of nodes required is 6.
```

## Compile & Execute
Compile command: &ensp; `$ make` \
Execute command: &ensp; &ensp; `$ ./Lab1 [input file] [output file]` \
e.g.&ensp; &ensp; &ensp; &ensp; &ensp; &ensp; &ensp; &ensp; &ensp; &ensp; &ensp; `$ ./Lab1 case1.txt out1.txt`

## How to get BDD figure
> Command
```bash
$ ./Lab1 [input file] [output file]
$ dot -Tpng [dot file] -o [png file]
```

> e.g.
```bash
$ ./Lab1 case1.txt out1.txt
$ dot -Tpng bdd.dot -o bdd.png
```
