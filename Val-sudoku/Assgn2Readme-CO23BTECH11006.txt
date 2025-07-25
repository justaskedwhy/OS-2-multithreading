File Tree:

The Src file contains the header and declaration of all functions used in script Assgn2Src-CO23BTECH11006.
To create executable run
    $ make
this will create a "main" executable file.
format to run the executable.
    $ ./main <input file path> <output file path> <lock type>
lock type has 3 types.
    tas : test and set,
    cas : compare and swap,
    casb : compare and swap (bounded).
e.g. command
    $ ./main inp.txt out.txt casb
    this will run main with input as "inp.txt" output as "out.txt" and lock type as "compare and swap bounded"
for the information on uses and definition of functions please refer chapter 1 of the report.

to make sudoku use the gen_sudoku.py file in ./Scripts folder.
command to create a sudoku
    $ python3 ./Scripts/gen_sudoku.py <number of threads> <size of sub-grid> <task increament> <input file name>
this will create the input file in the running directory.
e.g. command
    $ python3 ./Scripts/gen_sudoku.py 4 3 5 inp.txt
    this will create a input file named "inp.txt" with
    > 4 9 5
    > 6 3 8 7 1 5 2 4 9
    > 1 5 7 2 9 4 8 3 6
    > 9 4 2 8 6 3 7 5 1
    > 5 2 1 9 4 8 6 7 3
    > 4 8 9 6 3 7 1 2 5
    > 3 7 6 1 5 2 9 8 4
    > 8 6 4 3 7 1 5 9 2
    > 7 1 3 5 2 9 4 6 8
    > 2 9 5 4 8 6 3 1 7
    as the sudoku.

!!! you are advised not use any other python scripts in the ./Scripts folder !!! 