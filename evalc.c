#include <stdio.h>
#include <stdlib.h>
typedef struct Symbol Symbol;
typedef struct Tree Tree;

enum {
  NUMBER,
  VARIABLE,
  ADD,
  DIVIDE,
  MAX,
  ASSIGN
};

struct Symbol {
  int  value;
  char *name;
};

struct Tree {
  int    op;      /* operation code */
  int    value;   /* value if number */
  Symbol *symbol; /* symbol entry if variable */
  Tree   *left;
  Tree   *right;
};

int eval1(Tree *t);
int eval2(Tree *t);
int eval3(Tree *t);

int eval1(Tree *t) {
  int left, right;

  switch (t->op) {
    case NUMBER:
      return t->value;
    case VARIABLE:
      return t->symbol->value;
    case ADD:
      return eval1(t->left) + eval1(t->right);
    case DIVIDE:
      left = eval1(t->left);
      right = eval1(t->right);
      if (right == 0) {
        printf("divide by zero\n");
        exit(1);
      }
      return left / right;
    case MAX:
      left = eval1(t->left);
      right = eval1(t->right);
      return left > right ? left : right;
    case ASSIGN:
      t->left->symbol->value = eval1(t->right);
      return t->left->symbol->value;
  }

  printf("unsupported operation: %d\n", t->op);
  exit(1);
}

int maxop2(Tree *t) {
  int left, right;
  left = eval2(t->left);
  right = eval2(t->right);
  return left > right ? left : right;
}

int assignop2(Tree *t) {
  t->left->symbol->value = eval2(t->right);
  return t->left->symbol->value;
}

int addop2(Tree *t) {
  return eval2(t->left) + eval2(t->right);
}

int divop2(Tree *t) {
  int left, right;
  left = eval2(t->left);
  right = eval2(t->right);
  if (right == 0) {
    printf("divide by zero\n");
    exit(1);
  }
  return left / right;
}

int pushop2(Tree *t) {
  return t->value;
}

int pushsymop2(Tree *t) {
  return t->symbol->value;
}

int (*optab2[])(Tree *t) = {
  pushop2,    /* NUMBER */
  pushsymop2, /* VARIABLE */
  addop2,     /* ADD */
  divop2,     /* DIVIDE */
  maxop2,     /* MAX */
  assignop2,  /* ASSIGN */
};

int eval2(Tree *t) {
  return (*optab2[t->op])(t);
}

typedef union Code Code;
union Code {
  void   (*op)(void); /* function if operator */
  int    value;       /* value if number */
  Symbol *symbol;     /* Symbol entry if variable */
};

enum {
  NCODE = 64,
  NSTACK = 64,
};

Code code[NCODE];
int stack[NSTACK];
int stackp;
int pc; /* program counter */

void pushop3(void) {
  stack[stackp++] = code[pc++].value;
}

void pushsymop3(void) {

}

void divop3(void) {
  int left, right;

  right = stack[--stackp];
  left = stack[--stackp];
  if (right == 0) {
    printf("divide by zero\n");
    exit(1);
  }
  stack[stackp++] = left / right;
}

void addop3(void) {

}

void maxop3(void) {

}

int generate(int codep, Tree *t) {
  switch (t->op) {
    case NUMBER:
      code[codep++].op = pushop3;
      code[codep++].value = t->value;
      return codep;
    case VARIABLE:
      code[codep++].op = pushsymop3;
      code[codep++].symbol = t->symbol;
      return codep;
    case ADD:
      codep = generate(codep, t->left);
      codep = generate(codep, t->right);
      code[codep++].op = addop3;
      return codep;
    case DIVIDE:
      codep = generate(codep, t->left);
      codep = generate(codep, t->right);
      code[codep++].op = divop3;
      return codep;
    case MAX:
      return codep;
  }
  printf("unsupported operation: %d\n", t->op);
  exit(1);
}

int eval3(Tree *t) {
  pc = generate(0, t);
  code[pc].op = NULL;

  stackp = 0;
  pc = 0;
  while (code[pc].op != NULL) {
    (*code[pc++].op)();
  }
  return stack[0];
}

int main(void) {
  /* 
   * a = max(b, c/2);
   *
   *    =
   *  /   \
   * a    max
   *     /   \
   *    b     /
   *        /   \
   *       c     2
   */

  Symbol b;
  b.value = 6;
  b.name = "b";

  Symbol c;
  c.value = 4;
  c.name = "c";

  Symbol a;
  a.value = 4;
  a.name = "c";

  Tree avar;
  avar.op = VARIABLE;
  avar.symbol = &a;
  avar.left = NULL;
  avar.right = NULL;

  {
    Tree bvar;
    bvar.op = VARIABLE;
    bvar.symbol = &b;
    bvar.left = NULL;
    bvar.right = NULL;

    Tree cvar;
    cvar.op = VARIABLE;
    cvar.symbol = &c;
    cvar.left = NULL;
    cvar.right = NULL;

    Tree number2;
    number2.op = NUMBER;
    number2.value = 2;
    number2.left = NULL;
    number2.right = NULL;

    Tree divide;
    divide.op = DIVIDE;
    divide.left = &cvar;
    divide.right = &number2;

    Tree max;
    max.op = MAX;
    max.left = &bvar;
    max.right = &divide;

    Tree assign;
    assign.op = ASSIGN;
    assign.left = &avar;
    assign.right = &max;

    int result1 = eval1(&assign);
    printf("result1: %d\n", result1);

    int result2 = eval2(&assign);
    printf("result2: %d\n", result2);
  }

  return 0;
}
