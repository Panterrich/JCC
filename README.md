# JCC (JoJo Compiler Collection)

![alt text](images/README/JCC.png "program interface")
## It is the compiler for the JoJo programming language.

# Tests

I will provide some tests in comparison with the previous version of [my language](https://github.com/Panterrich/JPL "JPL"), which was executed on [my cpu](https://github.com/Panterrich/CPU "CPU"). 

For the tests, I will use [the following code](https://github.com/Panterrich/JCC/blob/master/files/Quadratic_1000.txt "Quadratic equation"), which calculates the quadratic equation.

The CPU was compiled with optimization -O3

## Test CPU

The CPU test was run for 10 million times of calculating the quadratic equation, the average time was 19 min 45 sec, and the number of callgrind instructions was 7,760,875,990,887. And this is despite the fact that all verifiers were disabled for the test

## Test JCC without optimizations

Since the executable file does not have any extra layers of abstractions, it is much faster than my cpu, so 2 tests were conducted.

For 10 million iterations, the results were as follows: the average time is 0.453 sec, and the number of callgrind instructions is 2,200,000,256

For 1 billion iterations, the results were as follows: the average time is 50,079 sec, and the number of callgrind instructions is 220,000,000,256

## Test JCC with optimizations

Here, the calls to the stack were reduced in order to increase the overall speed by removing unnecessary move xmm registers

For 10 million iterations, the results were as follows: the average time is 0.403 sec, and the number of callgrind instructions is 2,130,000,256

For 1 billion iterations, the results were as follows: the average time is 43,044 sec, and the number of callgrind instructions is 213,000,000,256

# Conclusion

As you can see, the program was sped up by orders of magnitude. If we start from some constant (and not a real value), then according to the callgrind instruction, the program accelerated by about 3600 times. 

Even for such a fairly highly optimized program, where there are no unnecessary operations, we could still get an acceleration of 3%, if we evaluate the callgrind instructions, and if we still start from the execution time, the program sped up more than 16%.



# Build
To start, clone the repository using the command

```
git clone https://github.com/Panterrich/JCC.git
```

Then compile the JCC using the command

```
make all
```
OK, we are ready to compile programs in the JPL language.

If you want to use the GUI, then use the command

```
./build/JCC
```

Else the console will be used
```
make run-rm NAME="file_path"
```
or 
```
make run-hg NAME="file_path"
```
Depending on the version of the language in which the program is written: Romanji or Hiragana

Your finished executable file will be waiting for you along the path file_path.elf
