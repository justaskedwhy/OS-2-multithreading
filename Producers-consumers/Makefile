all: prod_cons-sems-co23btech11006.cpp prod_cons-locks-co23btech11006.cpp
	g++ prod_cons-sems-co23btech11006.cpp Src/Assign3.hpp -o main_sems
	g++ prod_cons-locks-co23btech11006.cpp Src/Assign3.hpp -o main_locks
clean:
	-rm main_sems main_locks
	-rm *.o
	-rm *.gch
	-rm *.out
	-rm *.exe
	-rm *.log
sems: main_sems
	./main_sems inp.txt sems.out
locks: main_locks
	./main_locks inp.txt locks.out