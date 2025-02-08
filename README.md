# Operating System Scheduler and Memory Allocation Simulation

This project simulates an operating system's scheduler and memory allocation. The scheduler implements the following algorithms:
1. Shortest Job First (SJF)
2. Preemptive Highest Priority First (HPF)
3. Round Robin (RR)
4. Multilevel Feedback Queue (MLFQ)

The memory allocation is implemented using the **Buddy System** and is represented as a tree.

## Setup and Running the Project

### 1. Setting Up the Makefile
To run the project, you need to modify the `run` section of the `Makefile` to specify the scheduling algorithm and optional quantum time. Here's how the `Makefile` looks:

```makefile
build:
	gcc process_generator.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc scheduler.c -o scheduler.out
	gcc process.c -o process.out
	gcc test_generator.c -o test_generator.out

clean:
	rm -f *.out processes.txt

all: clean build

run:
	./process_generator.out processes.txt -sch 3 -q 5
```

processes.txt: This is the input file containing the processes.

-sch: Specifies the scheduling algorithm:

1. Shortest Job First (SJF)

2. Preemptive Highest Priority First (HPF)

3. Round Robin (RR)

4. Multilevel Feedback Queue (MLFQ)

-q: (Optional) Specifies the quantum time for RR and MLFQ.

###  Input File Format

The input file `processes.txt` should follow this format:

```
#id arrival runtime priority memsize
1 1 6 5 200
2 3 3 3 170
```

### Running the Project

To run the project, use the following command:

```
make run
```

### Output Files

The program generates the following output files:

#### `memory.log`

This file contains memory allocation and deallocation events in the following format:

```
#At time x allocated y bytes for process z from i to j
At time 1 allocated 200 bytes for process 1 from 0 to 255
At time 3 allocated 170 bytes for process 2 from 256 to 511
At time 6 freed 170 bytes from process 2 from 256 to 511
At time 10 freed 200 bytes from process 1 from 0 to 255
```

#### `scheduler.log`

This file contains the scheduler's events in the following format:

```
#At time x process y state arr w total z remain y wait k
At time 1 process 1 started arr 1 total 6 remain 6 wait 0
At time 3 process 1 stopped arr 1 total 6 remain 4 wait 0
At time 3 process 2 started arr 3 total 3 remain 3 wait 0
At time 6 process 2 finished arr 3 total 3 remain 0 wait 0 TA 3 WTA 1
At time 6 process 1 resumed arr 1 total 6 remain 4 wait 3
At time 10 process 1 finished arr 1 total 6 remain 0 wait 3 TA 9 WTA 1.5
```

#### `scheduler.perf`

This file contains performance metrics in the following format:

```
CPU utilization = 100.00%
Avg WTA = 1.25
Avg Waiting = 1.5
```

## This project was a part of an Operating System course in the Cairo University, Faculty of Engineering

## Credits: 

* Moaaz Emam
* Yara Senousy
* Ruaa Amr
* Mohamed Tarek