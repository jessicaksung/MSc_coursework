# MSc_coursework

This repository contains assignments completed for MSc Computer Science courses at the University of Bristol. 

Some files were provided by the instructors of the courses. These include files in general, files in simple_screen and files in test_files. 

## Happy Bookcases 

**Brief:** A bookcase is happy if (1) each shelf only has books of the same color or is empty and (2) all books of the same color are on the same shelf. Books can only be rearranged in the follow ways: 

* Only one book can be moved at a time 
* Only the rightmost book from a shelf can be moved 
* A book must move to the rightmost on its new shelf
* Each shelf has a maximum capacity of books 

Write a program to output the number of moves to make a bookcase happy. See details in exercise 7.3 of https://github.com/csnwc/Exercises-In-C.

**Contents:** 

* bookcase.c program reads in a definition file of the initial state of the bookcase and displays the number of moves to make the bookcase happy. argv[1] takes in a definition file from test_files. Optionally argv[2] can take ‘verbose’ to print the bookcase. 

* extension.c extends the bookcase.c program to print the bookcase in color and improves the algorithm to remove duplicates when searching for the solution in a breath-first fashion. 

* Makefile is an automated tool to compile and run the programs. 

* test_files contains definition files for the bookcase. 

* simple_screen contains supplementary files to display graphics on the screen. 

