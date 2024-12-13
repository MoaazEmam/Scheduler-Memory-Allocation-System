build:
	gcc process_generator.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc scheduler.c -o scheduler.out
	gcc process.c -o process.out
	gcc test_generator.c -o test_generator.out
	gcc priority_queue.c -o priority_queue.out
	gcc circular_queue.c -o circular_queue.out

clean:
	rm -f *.out  processes.txt

all: clean build

run:
	./process_generator.out
