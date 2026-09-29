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

### 2. Input File Format

The input file `processes.txt` should follow this format:

```
#id arrival runtime priority memsize
1 1 6 5 200
2 3 3 3 170
```

#id : the process id

arrival: the arrival time of the process

runtime: the expected runtime of the process

priority: the priority of the process range from 0 to 10 where 0 is the highest priority and 10 is the
lowest priority.

memsize: the expected memory size occupied by the process

### 3. Running the Project

To run the project, use the following command:

```
make run
```

## Output Files

The program generates the following output files:

### `memory.log`

This file contains memory allocation and deallocation events in the following format:

```
#At time x allocated y bytes for process z from i to j
At time 1 allocated 200 bytes for process 1 from 0 to 255
At time 3 allocated 170 bytes for process 2 from 256 to 511
At time 6 freed 170 bytes from process 2 from 256 to 511
At time 10 freed 200 bytes from process 1 from 0 to 255
```

### `scheduler.log`

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

### `scheduler.perf`

This file contains performance metrics in the following format:

```
CPU utilization = 100.00%
Avg WTA = 1.25
Avg Waiting = 1.5
```

## Design

- **Processes:** the process generator `fork`s and `exec`s the clock and the scheduler. The scheduler then `fork`s and `exec`s one process per job when that job is first scheduled.
- **IPC:** arrivals reach the scheduler over a System V message queue, and the clock is shared through shared memory. The scheduler preempts and resumes processes with `SIGSTOP`/`SIGCONT`. A finished process notifies the scheduler with `SIGUSR1`, and the generator signals end-of-input with `SIGUSR2`.
- **Memory:** a buddy allocator is represented as a binary tree over 1024 bytes. Each request gets the smallest power-of-two block that fits. Blocks split recursively on allocation, and two free buddies merge back into their parent on release.

## Sample run

Round Robin with quantum 5 (`-sch 3 -q 5`) on this input (`processes.txt`):

```
#id arrival runtime priority memsize
1   4   11  9  32
2	7	2	8  220
3	8	28	0 3
4	13	7	6 39
5	22	7	8   372
```

`scheduler.log` (first 12 of 28 events):

```
#At time x process y state arr w total z remain y wait k 
At time 4 process 1 STARTED arr 4 total 11 remain 11 wait 0
At time 9 process 1 STOPPED arr 4 total 11 remain 6 wait 0
At time 9 process 2 STARTED arr 7 total 2 remain 2 wait 2
At time 11 process 2 FINISHED arr 7 total 2 remain 0 wait 2 TA 4.00 WTA 2.00
At time 11 process 3 STARTED arr 8 total 28 remain 28 wait 3
At time 16 process 3 STOPPED arr 8 total 28 remain 23 wait 3
At time 16 process 1 RESUMED arr 4 total 11 remain 6 wait 7
At time 21 process 1 STOPPED arr 4 total 11 remain 1 wait 7
At time 21 process 4 STARTED arr 13 total 7 remain 7 wait 8
At time 26 process 4 STOPPED arr 13 total 7 remain 2 wait 8
At time 26 process 3 RESUMED arr 8 total 28 remain 23 wait 13
At time 31 process 3 STOPPED arr 8 total 28 remain 18 wait 13
...
```

`memory.log`:

```
#At time x allocated y bytes for process z from i to j 
At time 4 allocated 32 bytes for process 1 from 0 to 31 
At time 9 allocated 220 bytes for process 2 from 256 to 511 
At time 11 freed 220 bytes for process 2 from 256 to 511 
At time 11 allocated 3 bytes for process 3 from 32 to 35 
At time 21 allocated 39 bytes for process 4 from 64 to 127 
At time 32 freed 32 bytes for process 1 from 0 to 31 
At time 32 allocated 372 bytes for process 5 from 512 to 1023 
At time 39 freed 39 bytes for process 4 from 64 to 127 
At time 46 freed 372 bytes for process 5 from 512 to 1023 
At time 59 freed 3 bytes for process 3 from 32 to 35
```

`scheduler.perf`:

```
CPU utilization = 93.22 % 
Avg WTA = 2.70 
Avg Waiting = 15.60
```

## This project was a part of an Operating System course in the Cairo University, Faculty of Engineering

## Credits: 

* Moaaz Emam
* Yara Senousy
* Ruaa Amr
* Mohamed Tarek