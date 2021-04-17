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

* stack contains ADT for stack data structure.

* SDL contains supplmenetary files to render SDL on the screen.

* general contains files for using boolean type and managing memory. 


## Data Management System

**Brief:** Design and build a relational database server from scratch. The database server can (1) store entity-relationship data via a file system, (2) respond to client query requests and manipulate data accordingly and (3) validate the formal grammar of the query language as defined in the BNF. The database system must allow persistent storage of data and data is stored as TAB separated files.  

**Contents:**

* DBException contains custom exceptions for invalid query requests.

* DBServer.java is the database server.

* DBClient.java is the client that makes and receives queries from the server.

* DBQuery.java is the query output.

* DBTable.java is the data table.

* DBEntity.java is the entity or row of data.

* DBFileSystem.java is the system to read and write files and translate data between files and the database.

* DBTokenizer.java is the processor to break query requests into token to parse through.

* DBParser.java is the parser to validate the formal grammar.

* DBCondition.java is the processor that validates the conditions in query requests.

* DBCommand.java is the abstract class to hold attributes required by query commands. 

* AlterCommand.java, DropCommand.java, UseCommand.java, CreateCommand.java, InsertCommand.java, JoinCommand.java, SelectCommand.java and UpdateCommand.java are the individual query commands.



