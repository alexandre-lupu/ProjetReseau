all : Master
Master: Master.o
	gcc -o $@ $^ -lpthread

%.o : %.c
	gcc -c $< -lpthread

clean:
	rm *~ Master