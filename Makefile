all : Master Controleur Spy
Master: Master.o
	gcc -g -o $@ $^ -lpthread

Spy: Spy.o
	gcc -g -o $@ $^ -lpthread

Controleur: Controleur.o
	gcc -g -o $@ $^ -lpthread

%.o : %.c
	gcc -g -c $< -lpthread

clean:
	rm *~ Master *.o Controleur Spy