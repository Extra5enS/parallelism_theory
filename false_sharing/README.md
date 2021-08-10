# False sharing

Problem of false sharing includes fact that paralle varient of program may have less profite than line one

## Why flase sharing exist

False sharing problem connect with cache. both variables may place at the same cachline. If the value changes in one thread, it must also be changed in the cache of another (loading from memory). Example: 2 variables of 32bit each can fall into the same 64bit long cacheline. 

## Little update to avoid this problem

Let's fill the empty part of the line with unused variables. Then two variables will not be able to get into the same line and generate cashmises. Here we made a test that checked whether the performance of the algorithm tied to the cache improves.

## Resultes

![alt text](https://github.com/Extra5enS/parallelism_theory/blob/master/false_sharing/graph.jpg)

In this graph we can see 3 "lines".
* Purple line shows working of programme in single thread mode. This mode is quickest because this task is more simple for 1 thread in environment of many processes.
* Green shows multithread mode without optimization.
* Blue shows line with optimization. How you see, it's work, but don't avoid false sharing in full meaning.
