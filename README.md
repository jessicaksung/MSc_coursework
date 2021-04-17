# MSc_coursework

This repository contains assignments completed for MSc Computer Science courses at the University of Bristol. 

Some files were provided by the instructors of the courses. These include files in general, files in simple_screen and files in test_files. 

## Happy Bookcases 

**Brief:** A bookcase is happy if (1) each shelf only has books of the same color or is empty and (2) all books of the same color are on the same shelf. Books can only be rearranged in the follow ways: 

* Only one book can be moved at a time 
* Only the rightmost book from a shelf can be moved 
* A book must move to the rightmost on its new shelf
* Each shelf has a maximum capacity of books 

Write a program to output the number of moves to make a bookcase happy. 
See details in exercise 7.3 of https://github.com/csnwc/Exercises-In-C.

**Contents:** 

* bookcase.c program reads in a definition file of the initial state of the bookcase and displays the number of moves to make the bookcase happy. argv[1] takes in a definition file from test_files. Optionally argv[2] can take ‘verbose’ to print the bookcase. 

* extension.c extends the bookcase.c program to print the bookcase in color and improves the algorithm to remove duplicates when searching for the solution in a breath-first fashion. 

* Makefile is an automated tool to compile and run the programs. 

* test_files contains definition files for the bookcase. 

* simple_screen contains supplementary files to display graphics on the screen. 


## Polymorphic Hashing 

**Brief:** Implement polymorphic hashing in C. The keys and values will be of unknown type and the hash table must be resizable. 

See details in exercise 11.1 of https://github.com/csnwc/Exercises-In-C.

**Contents:**

* realloc contains the ADT for double-hashing. specific.h has the custom data structure, assoc.h has the method descriptions and realloc.c has the implementation. 

* general contains files for using boolean type and managing memory. 


## Turtle Graphics 
**Brief:** A “turtle” can be instructed around the screen to draw graphics. Implement a recursive descent parser that takes in an instruction program via argv[1]. If the program follows the formal grammar, there is no output. Else, an exit is made. Then, implement an interpreter to draw the turtle movements on the screen via SDL.  

The formal grammar and more details are found in exercise 12.3 of https://github.com/csnwc/Exercises-In-C.

**Contents:** 

* parser.c is the program that parses through the program and validates the formal grammar. 

* interpreter.c extends the parser to draw the instructions on the screen.

* Makefile is an automated tool to compile and run the programs. 

* Stack contains ADT for stack data structure.

* SDL contains supplmenetary files to render SDL on the screen.

* general contains files for using boolean type and managing memory. 


