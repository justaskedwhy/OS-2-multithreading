# OS Assignment - 3 : co23btech11006
### File Tree
```
Assgn3_CO23BTECH11006
├── Assgn3-Prod_Cons-problem-statement.pdf
├── Assign3Report_co23btech11006.pdf
├── Makefile
├── Scripts
│   └── plot_results2.py
├── Src
│   ├── Assign3.cpp
│   ├── Assign3.hpp
│   └── tasks.cpp
├── figs
│   ├── Thread Number Ratios_1.png
│   ├── Thread Number Ratios_2.png
│   ├── time Ratios_1.png
│   └── time Ratios_2.png
├── inp.txt
├── prod_cons-locks-co23btech11006.cpp
├── prod_cons-sems-co23btech11006.cpp
└── readme.md
```
### Make
-   To create the executable files for prod_cons-locks-co23btech11006.cpp and prod_cons-sems-co23btech11006.cpp can be done through Makefile present in the directory.
```make```
This will create the executable files for the tasks.
```make sems```
Will run the executable file for the given input file **inp.txt**
```make locks```
Same as before for locks and conditional variables implementation.
```make clean```
Will remove unwanted and newly created file by the executable files such as .out and .log files.

### Coustumize
-   If the user want to output and log the values to different files, we can use the executable file to do it.
``` <executable> <input file path> <output file path> <log file path : defalut value ("main.log")> ```
sepecify the respective input and output file for the executable file.