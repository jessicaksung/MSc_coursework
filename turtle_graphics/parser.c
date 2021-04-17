#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "general/bool.h"
#include "general/general.h"

#define EXPRLEN 500
#define WORDLEN 50
#define strsame(A, B) (strcmp(A, B) == 0)

struct program {
   char a[EXPRLEN][WORDLEN];
   int size;
   int index;
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
void  start(program *p);
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
/* reads contents of .ttl file and stores in struct -> a */
void readFile(FILE* fp, program *p);


int main(int argc, char* argv[]) {
  FILE* fp;
  program* prog = initProgram();

  /* test(); */
  if (argc < 2) {
    freeProgram(prog);
    on_error("no file provided");
  }
  if ((fp = fopen(argv[1], "r")) == NULL) {
    freeProgram(prog);
    on_error("cannot open file");
  }
  readFile(fp, prog);
  fclose(fp);

  start(prog);

  freeProgram(prog);
  exit(0);
  return 0;
}

program* initProgram(void) {
  program *p = (program*) ncalloc(1, sizeof(program));
  return p;
}


bool freeProgram(program *p) {
  if (p == NULL) {
    return true;
  }
  free(p);
  return true;
}

void clearExpressions(program *p) {
  int i = 0;

  while (i < p -> size) {
    strcpy(p -> a[i], "");
    i++;
  }
  p -> index = 0;
  p -> size = 0;
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
  if (strsame(p -> a[p -> index], "FD")) {
    fd(p);
  } else if (strsame(p -> a[p -> index], "LT")) {
    lt(p);
  } else if (strsame(p -> a[p -> index], "RT")) {
    rt(p);
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
  if (isNumber(p)) {
    return;
  } else if (isAlpha(p)) {
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
  return;
}


void loop(program *p) {
  if (!strsame(p -> a[p -> index], "DO")) {
    on_error("error at loop: invalid start to loop");
  }
  incrementIndex(p);
  var(p);
  incrementIndex(p);
  if (!strsame(p -> a[p -> index], "FROM")) {
    on_error("error at loop: missing FROM ");
  }
  incrementIndex(p);
  varum(p);
  incrementIndex(p);
  if (!strsame(p -> a[p -> index], "TO")) {
    on_error("error at loop: missing TO ");
  }
  incrementIndex(p);
  varum(p);
  incrementIndex(p);
  if (!strsame(p -> a[p -> index], "{")) {
    on_error("error at loop: missing { ");
  }
  incrementIndex(p);
  instructlst(p);
}

bool isOp(program *p) {
  char* word = p -> a[p -> index];

  if (strsame(word, "+") || strsame(word, "-") || strsame(word, "*") || strsame(word, "/")) {
    return true;
  }
  return false;
}

void polish(program *p) {
  if (strsame(p -> a[p -> index], ";")) {
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
  var(p);
  incrementIndex(p);
  if (!strsame(p -> a[p -> index], ":=")) {
    on_error("error at set: missing :=");
  }
  incrementIndex(p);
  polish(p);
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


void test(void) {
  program* prog = initProgram();

  assert(prog -> size == 0);
  assert(prog -> index == 0);

  /* test start */
  /* test error at start */
  strcpy(prog -> a[0], "}");
  prog -> size = prog -> size + 1;
  /* missing { */
  /* start(prog); */
  clearExpressions(prog);
  /* test simple start {} */
  strcpy(prog -> a[0], "{");
  prog -> size = prog -> size + 1;
  strcpy(prog -> a[1], "}");
  prog -> size = prog -> size + 1;
  start(prog);
  assert(prog -> index == 1);
  assert(prog -> size == 2);
  clearExpressions(prog);

  /* test isNumber function */
  strcpy(prog -> a[0], "0");
  assert(isNumber(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "9");
  assert(isNumber(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "10");
  assert(isNumber(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "-10");
  assert(!isNumber(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "-10.0");
  assert(!isNumber(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], ".5");
  assert(isNumber(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "2.5");
  assert(isNumber(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "a");
  assert(!isNumber(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "12345!");
  assert(!isNumber(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "0..5");
  assert(!isNumber(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "9.567.7!");
  assert(!isNumber(prog));
  clearExpressions(prog);

  /* test variations of {<FD LT RT> #} */
  /* {FD 10} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FD");
  strcpy(prog -> a[2], "10");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  start(prog);
  clearExpressions(prog);
  /* {LT 1234} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "LT");
  strcpy(prog -> a[2], "1234");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  start(prog);
  clearExpressions(prog);
  /* {RT 0} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "RT");
  strcpy(prog -> a[2], "0");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  start(prog);
  assert(prog -> index = 3);
  clearExpressions(prog);
  /* test error at intsruction {error #} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FORWARD");
  strcpy(prog -> a[2], "0");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  /* start(prog); */
  clearExpressions(prog);
  /* test error at varnum {FD error} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FD");
  strcpy(prog -> a[2], "-0");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  /* start(prog); */
  clearExpressions(prog);

  /* test isAlpha function */
  strcpy(prog -> a[0], "A");
  assert(isAlpha(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "Z");
  assert(isAlpha(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "@");
  assert(!isAlpha(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "12345");
  assert(!isAlpha(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "z");
  assert(!isAlpha(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "AB");
  assert(!isAlpha(prog));
  clearExpressions(prog);

  /* test variations of {<FD LT RT> LETTER} */
  /* {FD A} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FD");
  strcpy(prog -> a[2], "A");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  start(prog);
  clearExpressions(prog);
  /* {RT Z} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "RT");
  strcpy(prog -> a[2], "Z");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  start(prog);
  clearExpressions(prog);
  /* {LT M} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "LT");
  strcpy(prog -> a[2], "M");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  start(prog);
  clearExpressions(prog);

  /* test error at varum {<FD LT RT> error} */
  /* {FD a} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FD");
  strcpy(prog -> a[2], "a");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  /* start(prog); */
  clearExpressions(prog);
  /* {LT AZ} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "LT");
  strcpy(prog -> a[2], "AZ");
  strcpy(prog -> a[3], "}");
  prog -> size = prog -> size + 4;
  /* start(prog); */
  clearExpressions(prog);

  /* test multiple instruction calls from instrclst */
  /* {FD A RT 9} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FD");
  strcpy(prog -> a[2], "A");
  strcpy(prog -> a[3], "RT");
  strcpy(prog -> a[4], "9");
  strcpy(prog -> a[5], "}");
  prog -> size = prog -> size + 6;
  start(prog);
  clearExpressions(prog);
  /* {FD A RT 9 LT Z} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FD");
  strcpy(prog -> a[2], "A");
  strcpy(prog -> a[3], "RT");
  strcpy(prog -> a[4], "9");
  strcpy(prog -> a[5], "LT");
  strcpy(prog -> a[6], "Z");
  strcpy(prog -> a[7], "}");
  prog -> size = prog -> size + 8;
  start(prog);
  clearExpressions(prog);
  /* test error at varum {FD A RT -9} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FD");
  strcpy(prog -> a[2], "A");
  strcpy(prog -> a[3], "RT");
  strcpy(prog -> a[4], "-9");
  strcpy(prog -> a[5], "}");
  prog -> size = prog -> size + 6;
  /* start(prog); */
  clearExpressions(prog);
  /* test incomplete instrctlst {FD A RT 9 */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "FD");
  strcpy(prog -> a[2], "A");
  strcpy(prog -> a[3], "RT");
  strcpy(prog -> a[4], "9");
  prog -> size = prog -> size + 5;
  /* start(prog); */
  clearExpressions(prog);

  /* test each condition in DO loop function */
  /* {DO A FROM 0 to 10 {}} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "DO");
  strcpy(prog -> a[2], "A");
  strcpy(prog -> a[3], "FROM");
  strcpy(prog -> a[4], "0");
  strcpy(prog -> a[5], "TO");
  strcpy(prog -> a[6], "10");
  strcpy(prog -> a[7], "{");
  strcpy(prog -> a[8], "}");
  strcpy(prog -> a[9], "}");
  prog -> size = prog -> size + 10;
  start(prog);
  clearExpressions(prog);
  /* test error at instruction */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "DOO");
  prog -> size = prog -> size + 2;
  /* start(prog); */
  clearExpressions(prog);
  /* test error at var */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "DO");
  strcpy(prog -> a[2], "a");
  prog -> size = prog -> size + 3;
  /* start(prog); */
  clearExpressions(prog);
  /* test error at loop, incomplete */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "DO");
  strcpy(prog -> a[2], "A");
  prog -> size = prog -> size + 3;
  /* start(prog); */
  clearExpressions(prog);
  /* test error at varum */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "DO");
  strcpy(prog -> a[2], "A");
  strcpy(prog -> a[3], "FROM");
  strcpy(prog -> a[4], "-1");
  prog -> size = prog -> size + 5;
  /* start(prog); */
  clearExpressions(prog);
  /* test error at loop, missing TO */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "DO");
  strcpy(prog -> a[2], "A");
  strcpy(prog -> a[3], "FROM");
  strcpy(prog -> a[4], "1");
  strcpy(prog -> a[5], "TOO");
  prog -> size = prog -> size + 6;
  /* start(prog); */
  clearExpressions(prog);
  /* test error at loop, missing { */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "DO");
  strcpy(prog -> a[2], "A");
  strcpy(prog -> a[3], "FROM");
  strcpy(prog -> a[4], "1");
  strcpy(prog -> a[5], "TO");
  strcpy(prog -> a[6], "10");
  strcpy(prog -> a[7], "FD");
  prog -> size = prog -> size + 8;
  /* start(prog); */
  clearExpressions(prog);
  /* test completed loop but not instructlst */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "DO");
  strcpy(prog -> a[2], "A");
  strcpy(prog -> a[3], "FROM");
  strcpy(prog -> a[4], "1");
  strcpy(prog -> a[5], "TO");
  strcpy(prog -> a[6], "10");
  strcpy(prog -> a[7], "{");
  strcpy(prog -> a[8], "FD");
  strcpy(prog -> a[9], "10");
  prog -> size = prog -> size + 10;
  /* start(prog); */
  clearExpressions(prog);

  /* test isOp function */
  strcpy(prog -> a[0], "+");
  prog -> size = prog -> size + 1;
  assert(isOp(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "-");
  prog -> size = prog -> size + 1;
  assert(isOp(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "*");
  prog -> size = prog -> size + 1;
  assert(isOp(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "/");
  prog -> size = prog -> size + 1;
  assert(isOp(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "++");
  prog -> size = prog -> size + 1;
  assert(!isOp(prog));
  clearExpressions(prog);
  strcpy(prog -> a[0], "%");
  prog -> size = prog -> size + 1;
  assert(!isOp(prog));
  clearExpressions(prog);

  /* test polish function */
  /* test simplest ; case */
  strcpy(prog -> a[0], ";");
  prog -> size = prog -> size + 1;
  polish(prog);
  clearExpressions(prog);
  /* test multipe op calls */
  strcpy(prog -> a[0], "+");
  strcpy(prog -> a[1], "-");
  strcpy(prog -> a[2], "*");
  strcpy(prog -> a[3], "/");
  strcpy(prog -> a[4], ";");
  prog -> size = prog -> size + 5;
  polish(prog);
  clearExpressions(prog);
  /* test multiple varum calls */
  strcpy(prog -> a[0], "+");
  strcpy(prog -> a[1], "-");
  strcpy(prog -> a[2], "5");
  strcpy(prog -> a[3], "T");
  strcpy(prog -> a[4], "1910");
  strcpy(prog -> a[5], ";");
  prog -> size = prog -> size + 6;
  polish(prog);
  clearExpressions(prog);
  /* test error with multipe op calls without end */
  strcpy(prog -> a[0], "+");
  strcpy(prog -> a[1], "-");
  strcpy(prog -> a[2], "*");
  strcpy(prog -> a[3], "/");
  prog -> size = prog -> size + 4;
  /* polish(prog); */
  clearExpressions(prog);
  /* test error at varum */
  strcpy(prog -> a[0], "+");
  strcpy(prog -> a[1], "-");
  strcpy(prog -> a[2], "AB");
  strcpy(prog -> a[3], ";");
  prog -> size = prog -> size + 4;
  /* polish(prog); */
  clearExpressions(prog);

  /* test each condition in set function */
  /* test simplest case SET A := ; */
  strcpy(prog -> a[0], "SET");
  strcpy(prog -> a[1], "A");
  strcpy(prog -> a[2], ":=");
  strcpy(prog -> a[3], ";");
  prog -> size = prog -> size + 4;
  set(prog);
  clearExpressions(prog);
  /* SET A := 5C+; (revers polish) */
  strcpy(prog -> a[0], "SET");
  strcpy(prog -> a[1], "A");
  strcpy(prog -> a[2], ":=");
  strcpy(prog -> a[3], "5");
  strcpy(prog -> a[4], "C");
  strcpy(prog -> a[5], "+");
  strcpy(prog -> a[6], ";");
  prog -> size = prog -> size + 7;
  set(prog);
  clearExpressions(prog);
  /* SET A := 5 + C; */
  strcpy(prog -> a[0], "SET");
  strcpy(prog -> a[1], "A");
  strcpy(prog -> a[2], ":=");
  strcpy(prog -> a[3], "5");
  strcpy(prog -> a[4], "+");
  strcpy(prog -> a[5], "C");
  strcpy(prog -> a[6], ";");
  prog -> size = prog -> size + 7;
  set(prog);
  clearExpressions(prog);
  /* test error at SET, missing SET */
  strcpy(prog -> a[0], "A");
  prog -> size = prog -> size + 1;
  /* set(prog); */
  clearExpressions(prog);
  /* test error at var */
  strcpy(prog -> a[0], "SET");
  strcpy(prog -> a[1], "5");
  strcpy(prog -> a[2], ":=");
  strcpy(prog -> a[3], "5");
  strcpy(prog -> a[4], "C");
  strcpy(prog -> a[5], "+");
  strcpy(prog -> a[6], ";");
  prog -> size = prog -> size + 7;
  /* set(prog); */
  clearExpressions(prog);

  /* test error at set, missing := */
  strcpy(prog -> a[0], "SET");
  strcpy(prog -> a[1], "A");
  strcpy(prog -> a[2], "5");
  strcpy(prog -> a[3], "C");
  strcpy(prog -> a[4], "+");
  strcpy(prog -> a[5], ";");
  prog -> size = prog -> size + 6;
  /* set(prog); */
  clearExpressions(prog);
  /* test incomplete expression */
  strcpy(prog -> a[0], "SET");
  strcpy(prog -> a[1], "A");
  strcpy(prog -> a[2], ":=");
  strcpy(prog -> a[3], "5");
  strcpy(prog -> a[4], "C");
  strcpy(prog -> a[5], "+");
  strcpy(prog -> a[6], "-");
  prog -> size = prog -> size + 7;
  /* set(prog); */
  clearExpressions(prog);

  /* test set called from instruction */
  /* {SET E := 10 5 * A +; FD 10 RT 45} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "SET");
  strcpy(prog -> a[2], "E");
  strcpy(prog -> a[3], ":=");
  strcpy(prog -> a[4], "10");
  strcpy(prog -> a[5], "5");
  strcpy(prog -> a[6], "*");
  strcpy(prog -> a[7], "A");
  strcpy(prog -> a[8], "+");
  strcpy(prog -> a[9], ";");
  strcpy(prog -> a[10], "FD");
  strcpy(prog -> a[11], "10");
  strcpy(prog -> a[12], "RT");
  strcpy(prog -> a[13], "45");
  strcpy(prog -> a[14], "}");
  prog -> size = prog -> size + 15;
  start(prog);
  clearExpressions(prog);
  /* test incomplete set {SET E := 10 5 * A + FD 10 RT 45} */
  strcpy(prog -> a[0], "{");
  strcpy(prog -> a[1], "SET");
  strcpy(prog -> a[2], "E");
  strcpy(prog -> a[3], ":=");
  strcpy(prog -> a[4], "10");
  strcpy(prog -> a[5], "5");
  strcpy(prog -> a[6], "*");
  strcpy(prog -> a[7], "A");
  strcpy(prog -> a[8], "+");
  strcpy(prog -> a[9], "FD");
  strcpy(prog -> a[10], "10");
  strcpy(prog -> a[11], "RT");
  strcpy(prog -> a[12], "45");
  strcpy(prog -> a[13], "}");
  prog -> size = prog -> size + 14;
  /* start(prog); */

  freeProgram(prog);
  printf("passed\n");
}
