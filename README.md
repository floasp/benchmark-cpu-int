# benchmark-cpu-int
Benchmark programs that measures the time it takes to calculate some diophantine (integer) equations.

# What it does
The program runs calculations up to a certain number. The stopping number increases every run until the previous run took more than 200s. 
If that threshold is met, the next test starts or the program ends.

The calculations check, if the diophantine equation ```(a*a*c+b*b*a+c*c*b)/a*b*c``` results in an integer solution and outputs the solution in the terminal. 
The asymmetric version also writes out the thread that calculated it and the batch number that the thread is on. 
This should in theory show more batches on the more powerful cores of asymmetric CPUs.

The results are printed individually after each test run and collectively after the whole test completed. After each run two times durations are printed. 
The first one being the time duration calculated with the ```clock();``` function from the standard library 
and the second one with the ```gettimeofday();``` function from <sys/time.h>. While on Windows both numbers are almost identical, 
on Linux the first number might have to be divided by the number of cores, because the time is summed up for every core individually.

The collective results use the second time measurement  ```gettimeofday();``` which is the same on Windows and Linux.

All results plus the number of cores are written into a file called ```results.dat```.

## Versions
The **standard version** runs a single-core test, and after that a multi-core test with the threadcount set to the number of logical cpu cores in the system. 
The size of the batch to calculate is fixed for every core, which probably does make it not very useful on heterogenous architectures 
because weaker cores get the same amount of work to do as the powerful ones. 
(test needed, haven't got such a system yet)

The **asymmetric version** is made for heterogenous architectures such as Intels 12th gen processors or the ARM big.LITTLE architecture. It only runs a multi-core test with the 
threadcount also set to the number of cores in the system. The difference to the standard version is that each thread only does one step and then 
requests the next batch of numbers to calculate. Which takes a little longer than the standard version on the 3700X with Windows 
and is faster on the i5-3210M with Ubuntu.
As mentioned earlier, it should work much better on heterogenous architectures. (test needed, haven't got such a system yet)

# Compilation example with GCC
Compilation works the same for standard and asymmetric version:

GNU/Linux: ```gcc main.c utility.c -lpthread```

Windows: ```gcc main.c utility.c```

Mac: haven't got one to test it on.

# Some results
These tests were performed on various Systems that I found at home or at friends. The Systems vary greatly in age, 
build quality and cooling so they should be taken with a grain of salt.

## Standard version
| CPU                  | single core n=5000 | multi-core n=5000 | cores | threads | OS     | 
| -------------------- | ------------------ | ----------------- | ----- | ------- | ------ |
| AMD Ryzen 7 4800H    | 75.059s            | 9.492s            | 8     | 16      | Win 11 |
| AMD Ryzen 7 3700X    | 75.402s            | 9.812s            | 8     | 16      | Win 10 |
| Intel Core i9-9900K  | 157.770s           | 17.812s           | 8     | 16      | Win 11 |
| AMD Ryzen 5 4600H    | 116.976s           | 19.968s           | 6     | 12      | Win 11 |
| Intel Core i7-8750H  | 196.845s           | 36.808s           | 6     | 12      | Win 11 |
| Intel Core i7-4790k  | 214.893s           | 45.991s           | 4     | 8       | Win 10 |
| Intel Core i7-4720HQ | 250.753s           | 56.498s           | 4     | 8       | Win 10 |
| Raspberry Pi 4B @2GHz    | 348.067s       | 87.184s           | 4     | 4       | Ubuntu Server 18.04 |
| Raspberry Pi 4B @1.8GHz  | 397.975s       | 99.625s           | 4     | 4       | Ubuntu Server 18.04 |
| Intel Core i5-8265U  | 312.243s           | 114.838s          | 4     | 8       | Win 10 |
| Raspberry Pi 4B @2GHz    | 464.420s       | 116.225s          | 4     | 4       | Ubuntu Server 18.04 |
| Intel Core i5-3210M  | 321.585s           | 183.960s          | 2     | 4       | Ubuntu 22.04 |
| Intel Celeron N2940  | 777.989s           | 199.483s          | 2     | 4       | Win 10 |
| Intel M-5Y10         | 1041.906s          | 463.959s          | 2     | 4       | Win 8.1 |

<sub>Notes: Sorted by multi-core. Cores meaning physical cores and threads meaning logical cores. The Raspberry Pi 4B uses an ARM Broadcom BCM2711 processor. 
The i5-8265U is probably slow in multi-core because it has a power limit and no good cooling in the notebook.

## Asymmetric version
| CPU                  | multi-core n=5000 | cores | threads | OS     | 
| -------------------- | ----------------- | ----- | ------- | ------ |
| AMD Ryzen 7 4800H    | 9.657s            | 8     | 16      | Win 11 |
| AMD Ryzen 7 3700X    | 10.057s           | 8     | 16      | Win 10 |
| Intel Core i5-3210M  | 178.840           | 2     | 4       | Ubuntu 22.04 |


