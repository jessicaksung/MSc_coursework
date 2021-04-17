#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "general/bool.h"
#include "general/general.h"
#include "SDL/neillsdl2.h"
#include "stack/stack.h"
#include "stack/realloc/specific.h"

#define EXPRLEN 500
#define WORDLEN 50
#define strsame(A, B) (strcmp(A, B) == 0)
#define MILLISECONDDELAY 100
#define ALPHALEN 26
#define XSTART 300
#define YSTART 300

struct program {
   char a[EXPRLEN][WORDLEN];
   int size;
   int index;

   /* information to draw lines */
   int x1;
   int y1;
   int x2;
   int y2;
   double angle;
   double distance;

   /* stack to compute operations and hold values from varum */
   stack* s;

   /* array to store variables A-Z */
   double variables[ALPHALEN];
   int varIndex;

   SDL_Simplewin sw;
}; typedef struct program program;

/* tests each function */
void test(void);
/* initializes struct program */
program* initProgram(void);
/* frees struct */
bool freeProgram(program *p);
/* clears struct after testing different cases for a function */
void clearExpressions(program *p);
/* prints exprssions */
void printExpressions(program* p);
/* increments index by 1 to evaluate next word */
bool incrementIndex(program *p);
/* evalutes formal grammar for MAIN */
void start(program *p);
/* evalutes formal grammar for INSTRUCTLST */
void instructlst(program *p);
/* evalutes formal grammar for INSTRUCTION */
void instruction(program *p);
/* evalutes formal grammar for FD */
void fd(program *p);
/* evalutes formal grammar for LT */
void lt(program *p);
/* evalutes formal grammar for RT */
void rt(program *p);
/* evalutes formal grammar for VARUM */
void varum(program *p);
/* checks if a word is a valid number */
bool isNumber(program *p);
/* checks if a word is a valid letter A-Z */
bool isAlpha(program *p);
/* evalutes formal grammar for VAR */
void var(program *p);
/* evalutes formal grammar for DO LOOP */
void loop(program *p);
/* checks if a word is a valid operation symbol */
bool isOp(program *p);
/* evalutes formal grammar for POLISH */
void polish(program *p);
/* evalutes formal grammar for SET */
void set(program *p);
/* sets coordinates x2 and y2 */
void setCoordinates(program *p);
/* checks that the stack is empty (or operation is complete)*/
bool stackIsEmpty(program *p);
/* performs operations in reverse polish notation */
void performOp(program *p, char* word);
/* performs INSTRUCTLST called from DO LOOP */
void instructlstForLoop(program *p, int i, double count, int startIndex);
/* reads contents of .ttl file and stores in struct -> a */
void readFile(FILE* fp, program *p);
/* outputs line in SDL */
void drawLine(program *p);


int main(int argc, char* argv[]) {
  FILE* fp;
  program* prog;

  /* test(); */
  prog = initProgram();
  if (argc < 2) {
    on_error("no file provided");
  }
  if ((fp = fopen(argv[1], "r")) == NULL) {
    on_error("cannot open file");
  }
  readFile(fp, prog);
  fclose(fp);

  start(prog);

  do {
      Neill_SDL_Events(&(prog -> sw));
  } while (!(prog -> sw.finished));

  SDL_Quit();
  atexit(SDL_Quit);
  freeProgram(prog);
  exit(0);
  return 0;
}

program* initProgram(void) {
  int i;
  program *p = (program*) ncalloc(1, sizeof(program));

  p -> x1 = XSTART;
  p -> y1 = YSTART;
  p -> x2 = XSTART;
  p -> y2 = YSTART;

  /* initalize all variables at 0 */
  for (i = 0; i < ALPHALEN; i++) {
    p -> variables[i] = 0;
  }
  p -> s = stack_init();

  /* set up the window */
  Neill_SDL_Init(&(p -> sw));
  return p;
}

bool freeProgram(program *p) {
  if (p == NULL) {
    return true;
  }
  /* free stack */
  stack_free(p -> s);
  free(p);
  return true;
}

void clearExpressions(program *p) {
  int i = 0;
  double temp;

  while (i < p -> size) {
    strcpy(p -> a[i], "");
    i++;
  }
  p -> index = 0;
  p -> size = 0;
  p -> x1 = XSTART;
  p -> y1 = YSTART;
  p -> x2 = XSTART;
  p -> y2 = YSTART;
  p -> angle = 0;
  p -> distance = 0;
  for (i = 0; i < ALPHALEN; i++) {
    p -> variables[i] = 0;
  }
  p -> varIndex = 0;
  while (!stackIsEmpty(p)) {
    stack_pop(p -> s, &temp);
  }
}

void printExpressions(program *p) {
  int i = 0;

  printf("-----printing-----\n");
  while (i < p -> size) {
    printf("%s\n", p -> a[i]);
    i++;
  }
}

bool incrementIndex(program *p) {
  p -> index = p -> index + 1;

  if (p -> index >= p -> size) {
    on_error("grammar is incomplete");
  }
  return true;
}

void start(program *p) {
  if (!strsame(p -> a[p -> index], "{")) {
    on_error("invalid start to program");
  }
  incrementIndex(p);
  instructlst(p);
}

void instructlst(program *p) {
  if (strsame(p -> a[p -> index], "}")) {
    return;
  } else {
    instruction(p);
    incrementIndex(p);
    instructlst(p);
    return;
  }
  on_error("error at instructlst");
}

void instruction(program *p) {
  double temp = 0;
  if (strsame(p -> a[p -> index], "FD")) {
    fd(p);
    /* set distance */
    stack_pop(p -> s, &(p -> distance));
    /* set coordinates */
    setCoordinates(p);
    /* draw line */
    drawLine(p);
  } else if (strsame(p -> a[p -> index], "LT")) {
    lt(p);
    stack_pop(p -> s, &temp);
    /* adjust and set angle relative to x axis */
    p -> angle = p -> angle - temp;
  } else if (strsame(p -> a[p -> index], "RT")) {
    rt(p);
    stack_pop(p -> s, &temp);
    /* adjust and set angle relative to x axis */
    p -> angle = p -> angle + temp;
  } else if (strsame(p -> a[p -> index], "DO")) {
    loop(p);
  } else if (strsame(p -> a[p -> index], "SET")) {
    set(p);
  } else {
    on_error("error at instruction");
  }
}

void fd(program *p) {
  incrementIndex(p);
  varum(p);
}

void lt(program *p) {
  incrementIndex(p);
  varum(p);
}

void rt(program *p) {
  incrementIndex(p);
  varum(p);
}

void varum(program *p) {
  double val;

  if (isNumber(p)) {
    val = strtod(p -> a[p -> index], NULL);
    stack_push(p -> s, val);
    return;
  } else if (isAlpha(p)) {
    /* store index of the variable */
    val = p -> variables[(p -> a[p -> index][0]) - 'A'];
    stack_push(p -> s, val);
    return;
  }
  on_error("error at varum: invalid variable or number");
}

bool isNumber(program *p) {
  char* word = p -> a[p -> index];
  int i = 0;
  int countDecimal = 0;

  while (word[i] != '\0') {
    /* allows decimal but not negative numbers */
    if (((int) word[i] < 48 && (int) word[i] != 46) || (int) word[i] > 57) {
      return false;
    }
    if ((int) word[i] == 46) {
      countDecimal++;
    }
    i++;
  }

  /* if decimal, check that its valid with only one . */
  if (countDecimal > 1) {
    return false;
  }
  return true;
}

bool isAlpha(program *p) {
  char* word = p -> a[p -> index];

  /* must be upper-case letter */
  if ((int) word[0] < 65 || (int) word[0] > 90) {
    return false;
  }

  /* must be a single letter */
  if (word[1] != '\0') {
    return false;
  }
  return true;
}

void var(program *p) {
  char* word = p -> a[p -> index];

  /* must be upper-case letter */
  if ((int) word[0] < 65 || (int) word[0] > 90) {
    on_error("error at var: invalid variable");
  }

  /* must be a single letter */
  if (word[1] != '\0') {
    on_error("error at var: invalid variable");
  }

  p -> varIndex = word[0] - 'A';
  return;
}


void loop(program *p) {
  double count;
  int i;
  int startIndex;

  if (!strsame(p -> a[p -> index], "DO")) {
    on_error("error at loop: invalid start to loop");
  }
  incrementIndex(p);
  /* set varIndex */
  var(p);
  i = p -> varIndex;
  incrementIndex(p);
  if (!strsame(p -> a[p -> index], "FROM")) {
    on_error("error at loop: missing FROM ");
  }
  incrementIndex(p);
  /* pushed value to stack */
  varum(p);
  /* set variable to starting count */
  stack_pop(p -> s, &(p -> variables[i]));
  incrementIndex(p);
  if (!strsame(p -> a[p -> index], "TO")) {
    on_error("error at loop: missing TO ");
  }
  incrementIndex(p);
  /* pushed value to stack */
  varum(p);
  /* set ending count for variable */
  stack_pop(p -> s, &count);
  incrementIndex(p);
  if (!strsame(p -> a[p -> index], "{")) {
    on_error("error at loop: missing { ");
  }
  incrementIndex(p);
  /* sets index to loop back to */
  startIndex = p -> index;
  instructlstForLoop(p, i, count, startIndex);
}

void instructlstForLoop(program *p, int i, double count, int startIndex) {
  if (strsame(p -> a[p -> index], "}")) {
    if (p -> variables[i] < count) {
      /* increment counter */
      p -> variables[i] = p -> variables[i] + 1;
      /* reset index to the start of the loop */
      p -> index = startIndex;
      /* repeat */
      instructlstForLoop(p, i, count, startIndex);
    }
    return;
  } else {
    instruction(p);
    incrementIndex(p);
    instructlstForLoop(p, i, count, startIndex);
    return;
  }
  on_error("error at instructlst for loop");
}

bool isOp(program *p) {
  char* word = p -> a[p -> index];

  if (strsame(word, "+") || strsame(word, "-") || strsame(word, "*") || strsame(word, "/")) {
    /* performs operation */
    performOp(p, word);
    return true;
  }
  return false;
}

void performOp(program *p, char* word) {
  double operand1;
  double operand2;

  if (!stack_pop(p -> s, &operand2)) {
    on_error("no operand to operate on");
  }
  if (!stack_pop(p -> s, &operand1)) {
    on_error("no operand to operate on");
  }

  if (strsame(word, "+")) {
    /* push result to stack */
    stack_push(p -> s, operand1 + operand2);
  }
  if (strsame(word, "-")) {
    stack_push(p -> s, operand1 - operand2);
  }
  if (strsame(word, "*")) {
    stack_push(p -> s, operand1 * operand2);
  }
  if (strsame(word, "/")) {
    stack_push(p -> s, operand1 / operand2);
  }
}

void polish(program *p) {
  double temp;

  /* end of operation */
  if (strsame(p -> a[p -> index], ";")) {
    if (!stack_pop(p -> s, &temp)) {
      on_error("invalid operation");
    }
    /* checks if there are still operations left on the stack */
    if (!stackIsEmpty(p)) {
      on_error("invalid operation: stack not cleared");
    }
    p -> variables[p -> varIndex] = temp;
    return;
  }

  if (isOp(p)) {
    incrementIndex(p);
    polish(p);
    return;
  }
  varum(p);
  incrementIndex(p);
  polish(p);
}

void set(program *p) {
  if (!strsame(p -> a[p -> index], "SET")) {
    on_error("error at set: missing SET");
  }
  incrementIndex(p);
  /* set varIndex */
  var(p);
  incrementIndex(p);
  if (!strsame(p -> a[p -> index], ":=")) {
    on_error("error at set: missing :=");
  }
  incrementIndex(p);
  polish(p);
}

void setCoordinates(program *p) {
  /* convert angle to radian */
  double radian = (p -> angle) * (M_PI/180);

  /* reset x1 and y1 */
  p -> x1 = p -> x2;
  p -> y1 = p -> y2;

  /* set x2 and y2 */
  p -> x2 = (int) ((p -> distance) * cos(radian)) + p -> x1;
  p -> y2 = (int) ((p -> distance) * sin(radian)) + p -> y1;
}

bool stackIsEmpty(program *p) {
  if (p -> s -> size <= 0) {
    return true;
  }
  return false;
}

void readFile(FILE* fp, program* p) {
  char word[WORDLEN];
  int i = 0;

  while(fscanf(fp, "%s", word) != EOF) {
    strcpy(p -> a[i], word);
    i++;
    p -> size = p -> size + 1;
  }
}

void drawLine(program *p) {
  SDL_Delay(MILLISECONDDELAY);
  SDL_SetRenderDrawColor(p -> sw.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(p ->sw.renderer, p -> x1, p -> y1, p -> x2, p -> y2);

  /* draw line as program parses */
  Neill_SDL_UpdateScreen(&(p ->sw));
  Neill_SDL_Events(&(p -> sw));
}

/* in addition to the tests in parcer.c */
void test(void) {
  program* prog = initProgram();
  int i;
  double temp;

  /* test initial struct values */
  assert(prog -> x1 == 300);
  assert(prog -> y1 == 300);
  assert(prog -> x2 == 300);
  assert(prog -> y2 == 300);
  for (i = 0; i < ALPHALEN; i++) {
    assert(prog -> variables[i] < 0.01);
    assert(prog -> variables[i] > -0.01);
  }
  assert(prog -> angle < 0.01);
  assert(prog -> angle > -0.01);
  assert(prog -> distance < 0.01);
  assert(prog -> distance > -0.01);
  assert(prog -> varIndex == 0);

  /* test updates to struct direction and angle */
  /* {LT 45} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "LT");
  strcpy(prog -> a[2], "45");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  start(prog);
  assert(prog -> angle > -45.01);
  assert(prog -> angle < -44.99);
  clearExpressions(prog);
  /* {RT 30} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "RT");
  strcpy(prog -> a[2], "30");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  start(prog);
  assert(prog -> angle > 29.99);
  assert(prog -> angle < 30.01);
  clearExpressions(prog);
  /* {FD 20} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FD");
  strcpy(prog -> a[2], "20");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  start(prog);
  /* angle not set for FD */
  assert(prog -> angle < 0.01);
  assert(prog -> angle > -0.01);
  clearExpressions(prog);
  /* test updates to distance */
  /* {FD 20} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FD");
  strcpy(prog -> a[2], "20");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  start(prog);
  assert(prog -> angle < 0.01);
  assert(prog -> angle > -0.01);
  assert(prog -> distance < 20.01);
  assert(prog -> distance > 19.99);
  clearExpressions(prog);
  /* {FD 21.8} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FD");
  strcpy(prog -> a[2], "21.5");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  start(prog);
  assert(prog -> angle < 0.01);
  assert(prog -> angle > -0.01);
  assert(prog -> distance < 21.51);
  assert(prog -> distance > 21.49);
  clearExpressions(prog);
  /* {FD 21.8.} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FD");
  strcpy(prog -> a[2], "21.8.");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  /* error at varum */
  /* start(prog); */
  clearExpressions(prog);
  /* {LT 80 FD 100} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "LT");
  strcpy(prog -> a[2], "80");
  strcpy(prog -> a[3], "FD");
  strcpy(prog -> a[4], "100");
  strcpy(prog -> a[5], "}");
  prog -> size = prog -> size + 6;
  start(prog);
  assert(prog -> angle < -79.99);
  assert(prog -> angle > -80.01);
  assert(prog -> distance < 100.01);
  assert(prog -> distance > 99.99);
  clearExpressions(prog);
  /* {RT 80 FD 100 LT 50} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "RT");
  strcpy(prog -> a[2], "80");
  strcpy(prog -> a[3], "FD");
  strcpy(prog -> a[4], "100");
  strcpy(prog -> a[5], "LT");
  strcpy(prog -> a[6], "50");
  strcpy(prog -> a[7], "}");
  prog -> size = prog -> size + 8;
  start(prog);
  assert(prog -> angle > 29.99);
  assert(prog -> angle < 30.01);
  assert(prog -> distance > 99.99);
  assert(prog -> distance < 100.01);
  clearExpressions(prog);

  /* test updates to x1, y1, x2 and y2 */
  /* FD 10 relative to x axis from (300, 300) */
  prog -> distance = 10;
  setCoordinates(prog);
  assert(prog -> x1 == 300);
  assert(prog -> y1 == 300);
  assert(prog -> x2 == 310);
  assert(prog -> y2 == 300);
  /* 45 degrees from (310, 300), distance = 10 */
  prog -> angle = 45;
  setCoordinates(prog);
  assert(prog -> x2 == 317);
  assert(prog -> y2 == 307);
  /* 30 degrees from (317, 307), distance = 10 */
  prog -> angle = 30;
  setCoordinates(prog);
  assert(prog -> x2 == 325);
  assert(prog -> y2 == 311);
  /* FD 100 180 degrees relative to x axis from (300, 300) */
  prog -> distance = 100;
  prog -> x1 = 300;
  prog -> y1 = 300;
  prog -> x2 = 300;
  prog -> y2 = 300;
  prog -> angle = 180;
  setCoordinates(prog);
  assert(prog -> x2 == 200);
  assert(prog -> y2 == 300);
  /* FD 100 360 degrees relative to x axis from (300, 300) */
  prog -> distance = 100;
  prog -> x1 = 300;
  prog -> y1 = 300;
  prog -> x2 = 300;
  prog -> y2 = 300;
  prog -> angle = 360;
  setCoordinates(prog);
  assert(prog -> x2 == 400);
  assert(prog -> y2 == 300);
  /* FD 100 RT 420 degrees relative to x axis from (300, 300) */
  prog -> x1 = 300;
  prog -> y1 = 300;
  prog -> x2 = 300;
  prog -> y2 = 300;
  prog -> angle = 420;
  prog -> distance = 100;
  setCoordinates(prog);
  assert(prog -> x2 == 349);
  assert(prog -> y2 == 386);

  /* set varIndex */
  strcpy(prog -> a[0], "A");
  var(prog);
  assert(prog -> varIndex == 0);
  strcpy(prog -> a[0], "Z");
  var(prog);
  assert(prog -> varIndex == 25);
  strcpy(prog -> a[0], "a");
  /* invalid variable */
  /* var(prog); */
  strcpy(prog -> a[0], "E");
  var(prog);
  assert(prog -> varIndex == 4);

  /* test stackIsEmpty function */
  clearExpressions(prog);
  assert(stackIsEmpty(prog));
  assert(prog -> s -> size == 0);
  stack_push(prog -> s, 5);
  assert(!stackIsEmpty(prog));
  assert(prog -> s -> size == 1);

  /* test performOp function */
  /* check variables are updated */
  /* 5 + 5 = 10 */
  clearExpressions(prog);
  assert(prog -> varIndex == 0);
  stack_push(prog -> s, 5);
  stack_push(prog -> s, 5);
  performOp(prog, "+");
  stack_pop(prog -> s, &temp);
  assert(temp < 10.01);
  assert(temp > 9.99);
  assert(stackIsEmpty(prog));
  /* 7 - 5 = 2 */
  stack_push(prog -> s, 7);
  stack_push(prog -> s, 5);
  performOp(prog, "-");
  stack_pop(prog -> s, &temp);
  assert(temp < 2.01);
  assert(temp > 1.99);
  assert(stackIsEmpty(prog));
  /* 20 * 5 = 100 */
  stack_push(prog -> s, 20);
  stack_push(prog -> s, 5);
  performOp(prog, "*");
  stack_pop(prog -> s, &temp);
  assert(temp < 100.01);
  assert(temp > 99.99);
  assert(stackIsEmpty(prog));
  /* 10 / 2 = 5 */
  stack_push(prog -> s, 10);
  stack_push(prog -> s, 2);
  performOp(prog, "/");
  stack_pop(prog -> s, &temp);
  assert(temp < 5.01);
  assert(temp > 4.99);
  assert(stackIsEmpty(prog));
  /* 10 / 3 = 3 */
  prog -> varIndex = 2;
  stack_push(prog -> s, 10);
  stack_push(prog -> s, 3);
  performOp(prog, "/");
  stack_pop(prog -> s, &temp);
  assert(temp < 3.34);
  assert(temp > 3.32);
  assert(stackIsEmpty(prog));

  /* test performOp function called from set */
  /* SET A := 10 5 + 2 *; */
  clearExpressions(prog);
  strcpy(prog -> a[0], "SET");
  strcpy(prog -> a[1], "A");
  strcpy(prog -> a[2], ":=");
  strcpy(prog -> a[3], "10");
  strcpy(prog -> a[4], "5");
  strcpy(prog -> a[5], "+");
  strcpy(prog -> a[6], "2");
  strcpy(prog -> a[7], "*");
  strcpy(prog -> a[8], ";");
  prog -> size = prog -> size + 9;
  set(prog);
  /* assert(prog -> variables[0] == 30); */
  assert(stackIsEmpty(prog));
  /* SET A := 10 5 + 2 * / */
  clearExpressions(prog);
  strcpy(prog -> a[0], "SET");
  strcpy(prog -> a[1], "A");
  strcpy(prog -> a[2], ":=");
  strcpy(prog -> a[3], "10");
  strcpy(prog -> a[4], "5");
  strcpy(prog -> a[5], "+");
  strcpy(prog -> a[6], "2");
  strcpy(prog -> a[7], "*");
  strcpy(prog -> a[8], "/");
  strcpy(prog -> a[9], ";");
  prog -> size = prog -> size + 10;
  /* invalid operation */
  /* set(prog); */
  /* SET A := 2 10 / 2 5 + * */
  clearExpressions(prog);
  strcpy(prog -> a[0], "SET");
  strcpy(prog -> a[1], "A");
  strcpy(prog -> a[2], ":=");
  strcpy(prog -> a[3], "2");
  strcpy(prog -> a[4], "10");
  strcpy(prog -> a[5], "/");
  strcpy(prog -> a[6], "2");
  strcpy(prog -> a[7], "5");
  strcpy(prog -> a[8], "+");
  strcpy(prog -> a[9], "*");
  strcpy(prog -> a[10], ";");
  prog -> size = prog -> size + 11;
  set(prog);
  assert(prog -> variables[0] < 1.41);
  assert(prog -> variables[0] > 1.39);
  assert(stackIsEmpty(prog));
  /* SET A := 10 2 / 2 5 + ; */
  clearExpressions(prog);
  strcpy(prog -> a[0], "SET");
  strcpy(prog -> a[1], "A");
  strcpy(prog -> a[2], ":=");
  strcpy(prog -> a[3], "10");
  strcpy(prog -> a[4], "2");
  strcpy(prog -> a[5], "/");
  strcpy(prog -> a[6], "2");
  strcpy(prog -> a[7], "5");
  strcpy(prog -> a[8], "+");
  strcpy(prog -> a[9], ";");
  prog -> size = prog -> size + 10;
  /* invalid operation: stack not cleared after operations */
  /* set(prog); */

  /* test set called from start */
  /* {SET A := 10 2 * ; SET Z := 5 2 + 3 - ; } */
  clearExpressions(prog);
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "SET");
  strcpy(prog -> a[2], "A");
  strcpy(prog -> a[3], ":=");
  strcpy(prog -> a[4], "10");
  strcpy(prog -> a[5], "2");
  strcpy(prog -> a[6], "*");
  strcpy(prog -> a[7], ";");
  strcpy(prog -> a[8], "SET");
  strcpy(prog -> a[9], "Z");
  strcpy(prog -> a[10], ":=");
  strcpy(prog -> a[11], "5");
  strcpy(prog -> a[12], "2");
  strcpy(prog -> a[13], "+");
  strcpy(prog -> a[14], "3");
  strcpy(prog -> a[15], "-");
  strcpy(prog -> a[16], ";");
  strcpy(prog -> a[17], "}");
  prog -> size = prog -> size + 18;
  start(prog);
  assert(prog -> variables[0] < 20.01);
  assert(prog -> variables[0] > 19.99);
  assert(prog -> variables[25] < 4.01);
  assert(prog -> variables[25] > 3.99);
  /* {FD 10 RT 90 FD 50 SET B := 10 2 * 5 - 100 20 10 / + + ; } */
  clearExpressions(prog);
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FD");
  strcpy(prog -> a[2], "10");
  strcpy(prog -> a[3], "RT");
  strcpy(prog -> a[4], "90");
  strcpy(prog -> a[5], "FD");
  strcpy(prog -> a[6], "50");
  strcpy(prog -> a[7], "SET");
  strcpy(prog -> a[8], "B");
  strcpy(prog -> a[9], ":=");
  strcpy(prog -> a[10], "10");
  strcpy(prog -> a[11], "2");
  strcpy(prog -> a[12], "*");
  strcpy(prog -> a[13], "5");
  strcpy(prog -> a[14], "-");
  strcpy(prog -> a[15], "100");
  strcpy(prog -> a[16], "20");
  strcpy(prog -> a[17], "10");
  strcpy(prog -> a[18], "/");
  strcpy(prog -> a[19], "+");
  strcpy(prog -> a[20], "+");
  strcpy(prog -> a[21], ";");
  strcpy(prog -> a[22], "}");
  prog -> size = prog -> size + 23;
  start(prog);
  assert(prog -> distance < 50.01);
  assert(prog -> distance > 49.99);
  assert(prog -> angle > 89.99);
  assert(prog -> angle < 90.01);
  assert(prog -> variables[1] < 117.01);
  assert(prog -> variables[1] > 116.99);
  assert(prog -> x2 == 310);
  assert(prog -> y2 == 350);
  /* {FD 10 RT 45 SET B := 10 2 * 5 - 100 20 / + + ; } */
  clearExpressions(prog);
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FD");
  strcpy(prog -> a[2], "10");
  strcpy(prog -> a[3], "RT");
  strcpy(prog -> a[4], "90");
  strcpy(prog -> a[5], "SET");
  strcpy(prog -> a[6], "B");
  strcpy(prog -> a[7], ":=");
  strcpy(prog -> a[8], "10");
  strcpy(prog -> a[9], "2");
  strcpy(prog -> a[10], "*");
  strcpy(prog -> a[11], "5");
  strcpy(prog -> a[12], "-");
  strcpy(prog -> a[13], "100");
  strcpy(prog -> a[14], "20");
  strcpy(prog -> a[15], "/");
  strcpy(prog -> a[16], "+");
  strcpy(prog -> a[17], "+");
  strcpy(prog -> a[18], ";");
  strcpy(prog -> a[19], "}");
  prog -> size = prog -> size + 20;
  /* error at set with operation */
  /* start(prog); */

  /* test do loop */
  /* DO C FROM 1 TO 3 { FD 10 }, loop 3x*/
  clearExpressions(prog);
  strcpy(prog -> a[0], "DO");
  strcpy(prog -> a[1], "C");
  strcpy(prog -> a[2], "FROM");
  strcpy(prog -> a[3], "1");
  strcpy(prog -> a[4], "TO");
  strcpy(prog -> a[5], "3");
  strcpy(prog -> a[6], "{");
  strcpy(prog -> a[7], "FD");
  strcpy(prog -> a[8], "10");
  strcpy(prog -> a[9], "}");
  prog -> size = prog -> size + 10;
  loop(prog);
  assert(prog -> x2 == 330);
  assert(prog -> y2 == 300);
  assert(prog -> variables[2] < 3.01);
  assert(prog -> variables[2] > 2.99);
  /* DO D FROM 1 TO 1 { FD 50 }, loop once */
  clearExpressions(prog);
  strcpy(prog -> a[0], "DO");
  strcpy(prog -> a[1], "D");
  strcpy(prog -> a[2], "FROM");
  strcpy(prog -> a[3], "1");
  strcpy(prog -> a[4], "TO");
  strcpy(prog -> a[5], "1");
  strcpy(prog -> a[6], "{");
  strcpy(prog -> a[7], "FD");
  strcpy(prog -> a[8], "50");
  strcpy(prog -> a[9], "}");
  prog -> size = prog -> size + 10;
  loop(prog);
  assert(prog -> x2 == 350);
  assert(prog -> y2 == 300);
  assert(prog -> variables[3] < 1.01);
  assert(prog -> variables[3] > 0.99);
  /* test nested do loops */
  /* DO A FROM 1 TO 2 { FD 50 DO B FROM 1 TO 5 { FD 10 } }, inner loop 5x, outer loop 2x */
  clearExpressions(prog);
  strcpy(prog -> a[0], "DO");
  strcpy(prog -> a[1], "A");
  strcpy(prog -> a[2], "FROM");
  strcpy(prog -> a[3], "1");
  strcpy(prog -> a[4], "TO");
  strcpy(prog -> a[5], "2");
  strcpy(prog -> a[6], "{");
  strcpy(prog -> a[7], "FD"); /* starting point for outer loop */
  strcpy(prog -> a[8], "50");
  strcpy(prog -> a[9], "DO");
  strcpy(prog -> a[10], "B");
  strcpy(prog -> a[11], "FROM");
  strcpy(prog -> a[12], "1");
  strcpy(prog -> a[13], "TO");
  strcpy(prog -> a[14], "5");
  strcpy(prog -> a[15], "{");
  strcpy(prog -> a[16], "FD"); /* starting point for inner loop */
  strcpy(prog -> a[17], "10");
  strcpy(prog -> a[18], "}");
  strcpy(prog -> a[19], "}");
  prog -> size = prog -> size + 20;
  loop(prog);
  assert(prog -> x2 == 500);
  assert(prog -> y2 == 300);
  assert(prog -> variables[0] < 2.01);
  assert(prog -> variables[0] > 1.99);
  assert(prog -> variables[1] < 5.01);
  assert(prog -> variables[1] > 4.99);
  /* test do loop called from start */
  /* { DO Z FROM 0 TO 3 { FD 10 RT 90 } } */
  clearExpressions(prog);
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "DO");
  strcpy(prog -> a[2], "Z");
  strcpy(prog -> a[3], "FROM");
  strcpy(prog -> a[4], "0");
  strcpy(prog -> a[5], "TO");
  strcpy(prog -> a[6], "3");
  strcpy(prog -> a[7], "{");
  strcpy(prog -> a[8], "FD");
  strcpy(prog -> a[9], "10");
  strcpy(prog -> a[10], "LT");
  strcpy(prog -> a[11], "90");
  strcpy(prog -> a[12], "}");
  strcpy(prog -> a[13], "}");
  prog -> size = prog -> size + 14;
  start(prog);
  /* (300, 300) to (310, 300) to (310, 310) to (300, 310) to (300, 300) */
  assert(prog -> x2 == 300);
  assert(prog -> y2 == 300);
  assert(prog -> variables[25] < 3.01);
  assert(prog -> variables[25] > 2.99);
  /* test do loop, set, instructions */
  /* { DO A FROM 1 TO 3 { DO B FROM 1 to 1 { SET C := A 10 * ; SET D := B B + ; } } } */
  clearExpressions(prog);
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "DO");
  strcpy(prog -> a[2], "A");
  strcpy(prog -> a[3], "FROM");
  strcpy(prog -> a[4], "1");
  strcpy(prog -> a[5], "TO");
  strcpy(prog -> a[6], "3");
  strcpy(prog -> a[7], "{");
  strcpy(prog -> a[8], "DO");
  strcpy(prog -> a[9], "B");
  strcpy(prog -> a[10], "FROM");
  strcpy(prog -> a[11], "1");
  strcpy(prog -> a[12], "TO");
  strcpy(prog -> a[13], "1");
  strcpy(prog -> a[14], "{");
  strcpy(prog -> a[15], "SET");
  strcpy(prog -> a[16], "C");
  strcpy(prog -> a[17], ":=");
  strcpy(prog -> a[18], "A");
  strcpy(prog -> a[19], "10");
  strcpy(prog -> a[20], "*");
  strcpy(prog -> a[21], ";");
  strcpy(prog -> a[22], "SET");
  strcpy(prog -> a[23], "D");
  strcpy(prog -> a[24], ":=");
  strcpy(prog -> a[25], "B");
  strcpy(prog -> a[26], "B");
  strcpy(prog -> a[27], "+");
  strcpy(prog -> a[28], ";");
  strcpy(prog -> a[29], "}");
  strcpy(prog -> a[30], "}");
  strcpy(prog -> a[31], "}");
  prog -> size = prog -> size + 32;
  start(prog);
  assert(prog -> variables[0] < 3.01);
  assert(prog -> variables[0] > 2.99);
  assert(prog -> variables[1] < 1.01);
  assert(prog -> variables[1] > 0.99);
  assert(prog -> variables[2] < 30.01);
  assert(prog -> variables[2] > 20.99);
  assert(prog -> variables[3] < 2.01);
  assert(prog -> variables[3] > 1.99);

  SDL_Quit();
  atexit(SDL_Quit);
  freeProgram(prog);
  printf("passed\n");
}
