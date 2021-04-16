#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#define MAXHEIGHT 9
#define MAXWIDTH 9
#define ARRSIZE 80000
#define NOPARENT -1
#define EMPTY '.'
#define BLACK 'K'
#define RED 'R'
#define GREEN 'G'
#define YELLOW 'Y'
#define BLUE 'B'
#define MAGENTA 'M'
#define CYAN 'C'
#define WHITE 'W'
#define MAXCOLORS 8

struct bookcase{
  char shelf[MAXHEIGHT][MAXWIDTH];
  int h;
  int w;
  int parent;
};
typedef struct bookcase bookcase;
enum bool {false, true};
typedef enum bool bool;

/* testing functions */
void test(void);
/* stringify row for testing purposes */
void stringify(bookcase b[ARRSIZE], int index, int row, char str[MAXWIDTH + 2]);
/* initialize everything to start */
void clearBookcase(bookcase b[ARRSIZE]);
/* print book shelf */
void printBookcase(bookcase b[ARRSIZE], int index);
/* make copy of bookcase */
void copyBookcase(bookcase b[ARRSIZE], int childIndex, int parentIndex);
/* checks row to see if there's room to add */
bool hasSpace(bookcase b[ARRSIZE], int index, int row);
/* add book to end of row */
void addBook(bookcase b[ARRSIZE], int index, int row, char book);
/* remove book from end of row */
char removeBook(bookcase b[ARRSIZE], int index, int row);
/* checks if each row has one color or is empty */
bool sameColorRow(bookcase b[ARRSIZE], int index, int row);
/* checks if color is valid */
bool validColor(char color);
/* checks if each row has unique color */
bool uniqueColorRows(bookcase b[ARRSIZE], int index);
/* checks criteria for happy bookcase */
bool isHappy(bookcase b[ARRSIZE], int index);
/* initialize shelf to start of bookcases */
void initBookcases(bookcase b[ARRSIZE], char initialShelf[MAXHEIGHT][MAXWIDTH],
                                        int h, int w);
/* search to adds child bookcases from parent bookcase */
void addBookcase(bookcase b[ARRSIZE], int h, bool printFlag);
/* prints no solution or happy bookcases */
void printSolution(bookcase b[ARRSIZE], int index, bool happyFlag,
                                                   bool printFlag);
/* returns number of bookcases needed for happy bookcase */
int countSolution(bookcase b[ARRSIZE], int index);
/* reads file to get initial shelf */
void readShelf(FILE* fp, char shelf[MAXHEIGHT][MAXWIDTH]);

int main (int argc, char** argv) {
  static bookcase bookcases[ARRSIZE];
  char shelf[MAXHEIGHT][MAXWIDTH];
  int height = 0, width = 0;
  bool printFlag = false;
  FILE* fp;
  char line[MAXWIDTH];

  /* test(); */
  if (argc < 2) {
    fprintf(stderr, "please supply a file to read\n");
    exit(EXIT_FAILURE);
  }
  if (argc == 3 && strcmp(argv[2], "verbose") == 0) {
    printFlag = true;
  }
  if ((fp = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "cannot open file\n");
    exit(EXIT_FAILURE);
  }
  if (fgets(line, MAXWIDTH, fp) != NULL) {
    sscanf(line, "%d %d", &height, &width);
  }
  if (height < 0 || height > 9 || width < 0 || width > 9) {
    fprintf(stderr, "dimensions too large\n");
    exit(EXIT_FAILURE);
  }
  readShelf(fp, shelf);
  fclose(fp);
  clearBookcase(bookcases);
  initBookcases(bookcases, shelf, height, width);
  addBookcase(bookcases, height, printFlag);
  return 0;
}

void stringify(bookcase b[ARRSIZE], int index, int row,
                                               char str[MAXWIDTH + 2]) {
  int i;

  for (i = 0; i < b[index].w; i++) {
    str[i] = b[index].shelf[row][i];
  }
  str[i] = '\0';
}

void clearBookcase(bookcase b[ARRSIZE]) {
  int i, j, k;

  for (k = 0; k < ARRSIZE; k++) {
    b[k].h = 0;
    b[k].w = 0;
    b[k].parent = NOPARENT;
    for (j = 0; j < MAXHEIGHT; j++) {
      for(i = 0; i < MAXWIDTH; i++) {
        b[k].shelf[j][i] = EMPTY;
      }
    }
  }
}

void printBookcase(bookcase b[ARRSIZE], int index) {
   int i, j;

   for (j = 0; j < b[index].h; j++) {
     for (i = 0; i < b[index].w; i++) {
       printf("%c", b[index].shelf[j][i]);
     }
     printf("\n");
   }
   printf("\n");
}

void copyBookcase(bookcase b[ARRSIZE], int childIndex, int parentIndex) {
  int i, j;

  b[childIndex].h = b[parentIndex].h;
  b[childIndex].w = b[parentIndex].w;
  /* children from initial parent should reference it not default NOPARENT */
  if (b[parentIndex].parent == -1) {
    b[childIndex].parent = 0;
  } else {
    b[childIndex].parent = parentIndex;
  }
  for (j = 0; j < b[childIndex].h; j++) {
    for (i = 0; i < b[childIndex].w; i++) {
      b[childIndex].shelf[j][i] = b[parentIndex].shelf[j][i];
    }
  }
}

bool hasSpace(bookcase b[ARRSIZE], int index, int row) {
  int i;

  for (i = 0; i < b[index].w; i++) {
    if (b[index].shelf[row][i] == EMPTY) {
      return true;
    }
  }
  return false;
}

void addBook(bookcase b[ARRSIZE], int index, int row, char book) {
  int i = 0;

  while (b[index].shelf[row][i] != EMPTY) {
    i++;
  }
  b[index].shelf[row][i] = book;
}

char removeBook(bookcase b[ARRSIZE], int index, int row) {
  int i = 0;
  char book;

  if (b[index].shelf[row][i] == EMPTY) {
    book = EMPTY;
  }
  while (b[index].shelf[row][i] != EMPTY) {
    i++;
  }
  book = b[index].shelf[row][i - 1];
  b[index].shelf[row][i - 1] = EMPTY;
  return book;
}

bool sameColorRow(bookcase b[ARRSIZE], int index, int row) {
  int i;

  for (i = 0; i < (b[index].w - 1); i++) {
    /* it's valid if there's only 1 book so compare when there's 2+ */
    if (b[index].shelf[row][i + 1] != EMPTY) {
      if (b[index].shelf[row][i] != b[index].shelf[row][i + 1]) {
        return false;
      }
    }
  }
  return true;
}

bool validColor(char color) {
  char colors[MAXCOLORS] = {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA,
                            CYAN, WHITE};

  color = toupper(color);
  if (color != colors[0] && color != colors[1] && color != colors[2] &&
      color != colors[3] && color != colors[4] && color != colors[5] &&
      color != colors[6] && color != colors[7] && color != EMPTY) {
    return false;
  }
  return true;
}

bool uniqueColorRows(bookcase b[ARRSIZE], int index) {
  int i, j;
  int histo[MAXCOLORS] = {0};
  char color;

  for (j = 0; j < b[index].h; j++) {
    color = toupper(b[index].shelf[j][0]);
    switch(color) {
      case BLACK:
        histo[0]++;
        break;
      case RED:
        histo[1]++;
        break;
      case GREEN:
        histo[2]++;
        break;
      case YELLOW:
        histo[3]++;
        break;
      case BLUE:
        histo[4]++;
        break;
      case MAGENTA:
        histo[5]++;
        break;
      case CYAN:
        histo[6]++;
        break;
      case WHITE:
        histo[7]++;
    }
  }
  for (i = 0; i < MAXCOLORS; i++) {
    if (histo[i] > 1) {
      return false;
    }
  }
  return true;
}

bool isHappy(bookcase b[ARRSIZE], int index) {
  int j;
  char color;

  for (j = 0; j < b[index].h; j++) {
    /* checks that each row has books of one color or is empty */
    if (!sameColorRow(b, index, j)) {
      return false;
    }
    /* then checks that color is valid */
    color = b[index].shelf[j][0];
    if (!validColor(color)) {
      return false;
    }
  }
  /* then check that each row has unique color */
  if (!uniqueColorRows(b, index)) {
    return false;
  }
  return true;
}

void initBookcases(bookcase b[ARRSIZE], char initialShelf[MAXHEIGHT][MAXWIDTH],
                                        int h, int w) {
  int i, j;

  b[0].h = h;
  b[0].w = w;
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      b[0].shelf[j][i] = initialShelf[j][i];
    }
  }
}

void addBookcase(bookcase b[ARRSIZE], int h, bool printFlag) {
  int headIndex = 0, currentIndex = 0, j, k, happyIndex = 0;
  bool happyFlag = false;
  char book;

  if (isHappy(b, currentIndex)) {
    happyFlag = true;
  }
  while (!happyFlag && headIndex < (ARRSIZE - 1) &&
                       currentIndex < (ARRSIZE - 1)) {
    for (j = 0; j < h; j++) {
      if (b[headIndex].shelf[j][0] != EMPTY) {
        k = (j + 1) % h;
        while(k != j && !happyFlag) {
          if (hasSpace(b, headIndex, k)) {
            currentIndex++;
            if (currentIndex <= (ARRSIZE - 1)) {
              copyBookcase(b, currentIndex, headIndex);
              book = removeBook(b, currentIndex, j);
              addBook(b, currentIndex, k, book);
              if (isHappy(b, currentIndex)) {
                happyFlag = true;
                happyIndex = currentIndex;
              }
            }
          }
          k++;
          k = k % h;
        }
      }
    }
    headIndex++;
  }
  printSolution(b, happyIndex, happyFlag, printFlag);
}

void printSolution(bookcase b[ARRSIZE], int index, bool happyFlag,
                   bool printFlag) {
  int n;

  if (!happyFlag) {
    printf("No Solution?\n");
    return;
  }
  if (happyFlag) {
    printf("%d\n", countSolution(b, index));
  }
  if (printFlag) {
    n = index;
    printBookcase(b, n);
    n = b[n].parent;
    while (n != -1) {
      printBookcase(b, n);
      n = b[n].parent;
    }
  }
}

int countSolution(bookcase b[ARRSIZE], int index) {
  int n, count = 0;

  if (index == 0) {
    return 1;
  }
  n = index;
  while (n != -1) {
    n = b[n].parent;
    count++;
  }
  return count;
}

void readShelf(FILE* fp, char shelf[MAXHEIGHT][MAXWIDTH]) {
  int i = 0;
  char line[MAXWIDTH + 2];

  /* read line at a time as string */
  while (fgets(line, MAXWIDTH + 2, fp) != NULL) {
    line[strlen(line) - 1] = '\0';
    strcpy(shelf[i], line);
    i++;
  }
}

void test(void) {
  int i, j, k;
  int height = 3, width = 3;
  bookcase b1[ARRSIZE];
  char book1;
  char testColors[MAXCOLORS] = {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA,
                            CYAN, WHITE};
  char shelf1[MAXHEIGHT][MAXWIDTH];
  char str1[MAXWIDTH + 2];

  clearBookcase(b1);
  /* check board is cleared */
  for (k = 0; k < ARRSIZE; k++) {
    assert(b1[k].h == 0);
    assert(b1[k].w == 0);
    assert(b1[k].parent == NOPARENT);
    for (j = 0; j < MAXHEIGHT; j++) {
      for(i = 0; i < MAXWIDTH; i++) {
        assert(b1[k].shelf[j][i] == EMPTY);
      }
    }
  }
  /* set shelf for testing */
  b1[0].h = height;
  b1[0].w = width;
  b1[0].shelf[0][0] = YELLOW;
  b1[0].shelf[0][1] = EMPTY;
  b1[0].shelf[0][2] = EMPTY;
  b1[0].shelf[1][0] = BLUE;
  b1[0].shelf[1][1] = BLUE;
  b1[0].shelf[1][2] = YELLOW;
  b1[0].shelf[2][0] = YELLOW;
  b1[0].shelf[2][1] = BLUE;
  b1[0].shelf[2][2] = EMPTY;
  stringify(b1, 0, 0, str1);
  assert(strcmp(str1, "Y..") == 0);
  stringify(b1, 0, 1, str1);
  assert(strcmp(str1, "BBY") == 0);
  stringify(b1, 0, 2, str1);
  assert(strcmp(str1, "YB.") == 0);
  /* copy bookcase from starting bookcase */
  copyBookcase(b1, 1, 0);
  assert(b1[1].h == b1[0].h);
  assert(b1[1].h == b1[0].h);
  assert(b1[1].parent == 0);
  for (j = 0; j < b1[1].h; j++) {
    for (i = 0; i < b1[1].w; i++) {
      assert(b1[1].shelf[j][i] == b1[0].shelf[j][i]);
    }
  }
  /* copy bookcase from bookcase 1 to 5 */
  copyBookcase(b1, 5, 1);
  assert(b1[5].h == b1[1].h);
  assert(b1[5].h == b1[1].h);
  assert(b1[5].parent == 1);
  for (j = 0; j < b1[1].h; j++) {
    for (i = 0; i < b1[1].w; i++) {
      assert(b1[5].shelf[j][i] == b1[1].shelf[j][i]);
    }
  }
  /* check if partially full or full row has space */
  assert(hasSpace(b1, 0, 0));
  assert(!hasSpace(b1, 0, 1));
  assert(hasSpace(b1, 0, 2));
  /* remove book to create empty row */
  book1 = removeBook(b1, 0, 0);
  assert(book1 == YELLOW);
  stringify(b1, 0, 0, str1);
  assert(strcmp(str1, "...") == 0);
  assert(hasSpace(b1, 0, 0));
  /* add book to create full row */
  addBook(b1, 0, 2, BLUE);
  stringify(b1, 0, 2, str1);
  assert(strcmp(str1, "YBB") == 0);
  assert(!hasSpace(b1, 0, 2));
  /* checks if empty row is same color row */
  assert(sameColorRow(b1, 0, 0));
  /* checks same color rows, full and partially full */
  book1 = removeBook(b1, 0, 1);
  assert(book1 == YELLOW);
  addBook(b1, 0, 1, BLUE);
  stringify(b1, 0, 1, str1);
  assert(strcmp(str1, "BBB") == 0);
  assert(sameColorRow(b1, 0, 1));
  book1 = removeBook(b1, 0, 1);
  assert(book1 == BLUE);
  stringify(b1, 0, 1, str1);
  assert(strcmp(str1, "BB.") == 0);
  assert(sameColorRow(b1, 0, 1));
  /* checks different color rows, full and partially full */
  assert(!sameColorRow(b1, 0, 2));
  book1 = removeBook(b1, 0, 2);
  assert(book1 == BLUE);
  stringify(b1, 0, 2, str1);
  assert(strcmp(str1, "YB.") == 0);
  assert(!sameColorRow(b1, 0, 2));
  /* check valid colors */
  for (i = 0; i < MAXCOLORS; i++) {
    assert(validColor(testColors[i]));
  }
  assert(validColor(EMPTY));
  /* checks invalid colors */
  assert(!validColor('U'));
  assert(!validColor('1'));
  assert(!validColor('!'));
  /* check lower case colors */
  assert(validColor('r'));
  assert(validColor('g'));
  assert(validColor('b'));
  /* check unique color rows assuming it passed sameColorRow() and validColor() */
  b1[2].h = height;
  b1[2].w = width;
  /* empty shelf */
  assert(uniqueColorRows(b1, 2));
  /* unique colors */
  addBook(b1, 2, 0, GREEN);
  stringify(b1, 2, 0, str1);
  assert(strcmp(str1, "G..") == 0);
  assert(uniqueColorRows(b1, 2));
  addBook(b1, 2, 1, BLACK);
  stringify(b1, 2, 1, str1);
  assert(strcmp(str1, "K..") == 0);
  assert(uniqueColorRows(b1, 2));
  addBook(b1, 2, 2, RED);
  stringify(b1, 2, 2, str1);
  assert(strcmp(str1, "R..") == 0);
  assert(uniqueColorRows(b1, 2));
  /* check lower case color */
  book1 = removeBook(b1, 2, 2);
  assert(book1 == RED);
  addBook(b1, 2, 2, 'r');
  stringify(b1, 2, 2, str1);
  assert(strcmp(str1, "r..") == 0);
  assert(uniqueColorRows(b1, 2));
  /* duplicate colors */
  book1 = removeBook(b1, 2, 2);
  assert(book1 == 'r');
  addBook(b1, 2, 2, GREEN);
  assert(!isHappy(b1, 2));
  /* check if bookcase is happy */
  b1[3].h = height;
  b1[3].w = width;
  /* empty */
  assert(isHappy(b1, 3));
  /* happy */
  addBook(b1, 3, 0, RED);
  addBook(b1, 3, 1, GREEN);
  addBook(b1, 3, 2, BLUE);
  assert(isHappy(b1, 3));
  /* not happy bc row with different colors */
  addBook(b1, 3, 0, GREEN);
  assert(!isHappy(b1, 3));
  /* not happy bc row with invalid color */
  book1 = removeBook(b1, 3, 0);
  assert(book1 == GREEN);
  addBook(b1, 3, 0, 'U');
  assert(!isHappy(b1, 3));
  /* not happy bc rows with invalid or same colors */
  book1 = removeBook(b1, 3, 0);
  assert(book1 == 'U');
  book1 = removeBook(b1, 3, 0);
  assert(book1 == RED);
  addBook(b1, 3, 0, 'U');
  assert(!isHappy(b1, 3));
  book1 = removeBook(b1, 3, 0);
  assert(book1 == 'U');
  addBook(b1, 3, 0, BLUE);
  assert(!isHappy(b1, 3));
  /* test solution */
  printf("test empty shelf\n");
  clearBookcase(b1);
  b1[0].w = 3;
  b1[0].h = 3;
  addBookcase(b1, 3, true);
  assert(countSolution(b1, 1) == 1);
  printf("test 3x3 shelf\n");
  shelf1[0][0] = YELLOW;
  shelf1[0][1] = EMPTY;
  shelf1[0][2] = EMPTY;
  shelf1[1][0] = BLUE;
  shelf1[1][1] = BLUE;
  shelf1[1][2] = YELLOW;
  shelf1[2][0] = YELLOW;
  shelf1[2][1] = BLUE;
  shelf1[2][2] = EMPTY;
  clearBookcase(b1);
  initBookcases(b1, shelf1, 3, 3);
  assert(b1[0].h == 3);
  assert(b1[0].w == 3);
  assert(b1[0].shelf[0][0] == YELLOW);
  assert(b1[0].shelf[0][1] == EMPTY);
  assert(b1[0].shelf[0][2] == EMPTY);
  assert(b1[0].shelf[1][0] == BLUE);
  assert(b1[0].shelf[1][1] == BLUE);
  assert(b1[0].shelf[1][2] == YELLOW);
  assert(b1[0].shelf[2][0] == YELLOW);
  assert(b1[0].shelf[2][1] == BLUE);
  assert(b1[0].shelf[2][2] == EMPTY);
  /* add bookcases and check solution */
  addBookcase(b1, 3, true);
  assert(countSolution(b1, 68) == 4);
  /* test for bookcase that's already happy */
  printf("test already happy shelf\n");
  shelf1[0][0] = CYAN;
  shelf1[0][1] = CYAN;
  shelf1[0][2] = EMPTY;
  shelf1[1][0] = BLUE;
  shelf1[1][1] = BLUE;
  shelf1[1][2] = EMPTY;
  clearBookcase(b1);
  initBookcases(b1, shelf1, 2, 3);
  addBookcase(b1, 2, true);
  assert(countSolution(b1, 0) == 1);
  /* test no solution */
  printf("test no solution\n");
  shelf1[0][0] = BLUE;
  shelf1[0][1] = BLUE;
  shelf1[0][2] = BLUE;
  shelf1[1][0] = BLUE;
  shelf1[1][1] = BLUE;
  shelf1[1][2] = BLUE;
  clearBookcase(b1);
  initBookcases(b1, shelf1, 2, 3);
  addBookcase(b1, 2,true);
  /* test larger bookcase */
  printf("test 5x5 shelf\n");
  shelf1[0][0] = RED;
  shelf1[0][1] = RED;
  shelf1[0][2] = EMPTY;
  shelf1[0][3] = EMPTY;
  shelf1[0][4] = EMPTY;
  shelf1[1][0] = GREEN;
  shelf1[1][1] = GREEN;
  shelf1[1][2] = EMPTY;
  shelf1[1][3] = EMPTY;
  shelf1[1][4] = EMPTY;
  shelf1[2][0] = GREEN;
  shelf1[2][1] = RED;
  shelf1[2][2] = EMPTY;
  shelf1[2][3] = EMPTY;
  shelf1[2][4] = EMPTY;
  shelf1[3][0] = BLUE;
  shelf1[3][1] = BLACK;
  shelf1[3][2] = EMPTY;
  shelf1[3][3] = EMPTY;
  shelf1[3][4] = EMPTY;
  shelf1[4][0] = BLACK;
  shelf1[4][1] = EMPTY;
  shelf1[4][2] = EMPTY;
  shelf1[4][3] = EMPTY;
  shelf1[4][4] = EMPTY;
  clearBookcase(b1);
  initBookcases(b1, shelf1, 5, 5);
  addBookcase(b1, 5, true);
  /* test full bookcase, no solution */
  printf("test full shelf\n");
  shelf1[0][0] = RED;
  shelf1[0][1] = RED;
  shelf1[1][0] = RED;
  shelf1[1][1] = RED;
  clearBookcase(b1);
  initBookcases(b1, shelf1, 2, 2);
  addBookcase(b1, 2, true);
  printf("---testing ended---\n");
}
