all: ./Src/Src_header.o
	g++ Assgn2Src-CO23BTECH11006.cpp ./Src/Src_header.o -o main
./Src/Src_header.o : ./Src/Src_header.cpp ./Src/Src_header.h
	g++ ./Src/Src_header.cpp -c
gen:
	python3 ./Scripts/gen_sudoku.py 8 3 20 inp.txt
clean:
	-rm inp.*
	-rm out.*
	-rm main