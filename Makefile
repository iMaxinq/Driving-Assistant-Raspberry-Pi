C:: 
	gcc -c devices.c; 
	gcc -fopenmp -c threads.c; 
	gcc -o threads threads.o devices.o -lrt -lwiringPi -lpthread -lm -fopenmp

clean::
	rm *.o threads

all:: C
