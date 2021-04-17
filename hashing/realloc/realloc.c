#include "specific.h"
#include "assoc.h"
#include "../general/bool.h"
#define PRIME 11

/* tests functions */
void test(void);
/* returns index for hash table */
unsigned int hash(unsigned char *key, int capacity, int keysize);
/* returns incrementing probing size for double hashing */
unsigned int probeSize(unsigned char* key, int keysize);
/* increases the capacity of hash table by SCALEFACTOR */
bool resize(assoc** a);
/* rehash key-data pairs in old structure to new stucture */
void rehash(assoc* a, assoc* b);
/* checks if number is prime or not */
bool isPrime(int n);
/* returns current or next prime number */
int findNextPrime(int n);
/* checks if the key to insert is already in the hash table */
bool isDuplicate(assoc** a, void* key, int index, int probe);

assoc* assoc_init(int keysize) {
  assoc* a = (assoc*) ncalloc(1, sizeof(assoc));

  if (keysize < 0 ) {
    on_error("enter a positive keysize");
  }
  a -> capacity = INITSIZE;
  a -> table = (dataframe*) ncalloc(a -> capacity, sizeof(dataframe));
  a -> keysize = keysize;
  return a;
}

unsigned int assoc_count(assoc* a) {
  if (a == NULL) {
    return 0;
  }
  return a -> size;
}

void assoc_free(assoc* a) {
  free(a -> table);
  free(a);
}

void assoc_insert(assoc** a, void* key, void* data) {
  int index = hash((unsigned char*) key, (*a) -> capacity, (*a) -> keysize);
  int probe = probeSize((unsigned char*) key, (*a) -> keysize);
  assoc* temp;

  if (*a == NULL) {
    on_error("invalid structure, cannot insert key-data pair");
  }

  /* resize if at 70% capacity to improve efficiency of hashing */
  if ((*a) -> size >= ((*a) -> capacity * 0.70)) {
    temp = *a;
    resize(a);
    rehash(temp, *a);
    assoc_free(temp);
  }

  /* if key is not a duplicate, add to hash table */
  if (!isDuplicate(a, key, index, probe)) {
    if ((*a) -> table[index].key == NULL) {
        (*a) -> table[index].key = key;
        (*a) -> table[index].data = data;
        (*a) -> size++;
    } else {
      while ((*a) -> table[index].key != NULL) {
        index = (index + probe) % (*a) -> capacity;
      }
      (*a) -> table[index].key = key;
      (*a) -> table[index].data = data;
      (*a) -> size++;
    }
  }
}

void* assoc_lookup(assoc* a, void* key) {
  unsigned int count = 0;
  int index = hash((unsigned char*) key, a -> capacity, a -> keysize);
  int probe = probeSize((unsigned char*) key, a -> keysize);

  if (a == NULL) {
    on_error("structure must be valid");
  }

  if (a -> table[index].key == NULL) {
    return NULL;
  }

  while (count <= a -> capacity) {
    if (a -> table[index].key == NULL) {
      return NULL;
    } else {
      /* for string */
      if ((a -> keysize) == 0) {
        if (strcmp(a -> table[index].key, key) == 0) {
          return a -> table[index].data;
        }
      } else {
        /* for all other types */
        if (memcmp(a -> table[index].key, key, a -> keysize) == 0) {
          return a -> table[index].data;
        }
      }
    }
    count++;
    index = (index + probe) % a -> capacity;
  }
  return NULL;
}

/**********************/
/* Auxillary Functons */
/**********************/
/* adapted from http://www.cse.yorku.ca/~oz/hash.html */
unsigned int hash(unsigned char *key, int capacity, int keysize) {
     unsigned long hash = 5381;
     int c;
     int i = 0;

     /* for string */
     if (keysize == 0) {
       while((c = *key++)) {
         hash = ((hash << 5) + hash) + c;
       }
     } else {
       /* for all other types */
       while (i < keysize) {
         c = *key++;
         hash = ((hash << 5) + hash) + c;
         i++;
       }
     }
     return (unsigned int) hash % capacity;
 }

 /* adapted from https://www.geeksforgeeks.org/double-hashing/ */
 unsigned int probeSize(unsigned char* key, int keysize) {
   unsigned long hash = 5381;
   int c;
   int i = 0;

   /* for string */
   if (keysize == 0) {
     while((c = *key++)) {
       hash = ((hash << 5) + hash) + c;
     }
   } else {
     /* for all other types */
     while (i < keysize) {
       c = *key++;
       hash = ((hash << 5) + hash) + c;
       i++;
     }
   }
   return (unsigned int) PRIME - (hash % PRIME);
}

bool resize(assoc** a) {
  int newCapacity = (*a) -> capacity * SCALEFACTOR;
  assoc* b = (assoc*) ncalloc(1, sizeof(assoc));

  if (!isPrime(newCapacity)) {
    newCapacity = findNextPrime((*a) -> capacity * SCALEFACTOR);
  }
  b -> capacity = newCapacity;
  b -> table = (dataframe*) ncalloc(b -> capacity, sizeof(dataframe));
  b -> keysize = (*a) -> keysize;
  b -> size = 0;
  *a = b;
  return true;
}

void rehash(assoc* a, assoc* b) {
  unsigned int i;

  if (a == NULL || b == NULL) {
    on_error("structures must be valid");
  }
  for(i = 0; i < a -> capacity; i++) {
    if (a -> table[i].key != NULL) {
      assoc_insert(&b, a -> table[i].key, a -> table[i].data);
    }
  }
}

bool isPrime(int n) {
  int i;

  for (i = 2; i < n; i++) {
    if (n % i == 0) {
      return false;
    }
  }
  return true;
}

int findNextPrime(int n) {
  if (n < 0) {
    return 1;
  }
  while (!isPrime(n)) {
    n++;
  }
  return n;
}

bool isDuplicate(assoc** a, void* key, int index, int probe) {
  unsigned int count = 0;

  if (*a == NULL) {
    on_error("structure must be valid");
  }
  if ((*a) -> table[index].key == NULL) {
    return false;
  }
  while (count <= (*a) -> capacity) {
    if ((*a) -> table[index].key == NULL) {
      return false;
    } else {
      /* for string */
      if (((*a) -> keysize) == 0) {
        if (strcmp((*a) -> table[index].key, key) == 0) {
          return true;
        }
      } else {
        /* for all other types */
        if (memcmp((*a) -> table[index].key, key, (*a) -> keysize) == 0) {
          return true;
        }
      }
    }
    count++;
    index = (index + probe) % (*a) -> capacity;
  }
  return false;
}

void test(void) {
  assoc* a;
  char* str = "abcd";
  double i[10];
  char c[10];

  a = assoc_init(0);
  assert(assoc_count(a) == 0);
  assert(assoc_count(NULL) == 0);
  assert(a -> capacity == INITSIZE);
  assert(a -> keysize == 0);
  assert(hash((unsigned char*) str, a -> capacity, a -> keysize) == 10);
  assoc_insert(&a, str, NULL);
  assert(strcmp(a -> table[10].key, "abcd") == 0);
  assert(a -> table[10].data == NULL);
  assert(assoc_count(a) == 1);
  assoc_insert(&a, "test", NULL);
  assert(assoc_count(a) == 2);
  assoc_insert(&a, "a", NULL);
  assert(assoc_count(a) == 3);
  assoc_insert(&a, "b", NULL);
  assert(assoc_count(a) == 4);
  assoc_insert(&a, "c", NULL);
  assert(assoc_count(a) == 5);
  assoc_insert(&a, "d", NULL);
  assert(assoc_count(a) == 6);
  assoc_insert(&a, "e", NULL);
  assert(assoc_count(a) == 7);
  assoc_insert(&a, "f", NULL);
  assert(assoc_count(a) == 8);
  assoc_insert(&a, "g", NULL);
  assert(assoc_count(a) == 9);
  assoc_insert(&a, "h", NULL);
  assert(assoc_count(a) == 10);
  assoc_insert(&a, "i", NULL);
  assert(assoc_count(a) == 11);
  assoc_insert(&a, "j", NULL);
  assert(assoc_count(a) == 12);
  assoc_insert(&a, "k", NULL);
  assert(assoc_count(a) == 13);
  assoc_insert(&a, "l", NULL);
  assert(assoc_count(a) == 14);
  assoc_insert(&a, "m", NULL);
  assert(assoc_count(a) == 15);
  assoc_insert(&a, "n", NULL);
  assert(assoc_count(a) == 16);
  assoc_insert(&a, "o", NULL);
  assert(assoc_count(a) == 17);
  assoc_insert(&a, "p", NULL);
  assert(assoc_count(a) == 18);
  assoc_insert(&a, "q", NULL);
  assert(assoc_count(a) == 19);
  assoc_insert(&a, "r", NULL);
  assert(assoc_count(a) == 20);
  assoc_insert(&a, "holiday", "match");
  assert(assoc_count(a) == 21);
  assoc_insert(&a, "cheers", "match");
  assert(assoc_count(a) == 22);
  assert(a -> capacity == 37);
  assert(strcmp((char*)assoc_lookup(a, "cheers"), "match") == 0);
  assert(strcmp((char*)assoc_lookup(a, "holiday"), "match") == 0);
  assert(assoc_lookup(a, "r") == NULL);

  assert(isPrime(1));
  assert(isPrime(2));
  assert(isPrime(3));
  assert(!isPrime(4));
  assert(isPrime(5));
  assert(findNextPrime(1) == 1);
  assert(findNextPrime(2) == 2);
  assert(findNextPrime(3) == 3);
  assert(findNextPrime(4) == 5);
  assert(findNextPrime(33) == 37);
  assert(findNextPrime(100) == 101);

  assert(!isDuplicate(&a, "s", 32, 1));
  assert(!isDuplicate(&a, "s", 33, 1));
  assert(isDuplicate(&a, "d", 33, 1));
  assert(isDuplicate(&a, "holiday", 0, 31));
  assoc_free(a);

  a = assoc_init(sizeof(double));
  i[0] = 1.0;
  i[1] = 3.5;
  i[2] = 6.0;
  i[3] = 7.85;
  i[4] = 0.0;
  i[5] = 99.0;
  i[6] = 9.6;
  i[7] = 7.0;
  i[8] = 8.85;
  i[9] = 19.0;
  assoc_insert(&a, &i[0], &i[0]);
  assert(assoc_count(a) == 1);
  assoc_insert(&a, &i[1], &i[1]);
  assert(assoc_count(a) == 2);
  assoc_insert(&a, &i[2], &i[2]);
  assert(assoc_count(a) == 3);
  assoc_insert(&a, &i[3], NULL);
  assert(assoc_count(a) == 4);
  assoc_insert(&a, &i[4], &i[4]);
  assert(assoc_count(a) == 5);
  assoc_insert(&a, &i[5], &i[5]);
  assert(assoc_count(a) == 6);
  assoc_insert(&a, &i[6], &i[6]);
  assert(assoc_count(a) == 7);
  assert(assoc_lookup(a, &i[0]) == &i[0]);
  assert(assoc_lookup(a, &i[5]) == &i[5]);
  assert(assoc_lookup(a, &i[6]) == &i[6]);
  assert(assoc_lookup(a, &i[3]) == NULL);
  assoc_free(a);

  a = assoc_init(sizeof(char));
  c[0] = 'a';
  c[1] = 'b';
  c[2] = 'e';
  c[3] = 'f';
  c[4] = 'h';
  c[5] = 's';
  c[6] = 't';
  c[7] = 'c';
  c[8] = 'g';
  c[9] = 'q';
  assoc_insert(&a, &c[0], NULL);
  assert(assoc_count(a) == 1);
  assoc_insert(&a, &c[1], NULL);
  assert(assoc_count(a) == 2);
  assoc_insert(&a, &c[2], NULL);
  assert(assoc_count(a) == 3);
  assoc_insert(&a, &c[3], NULL);
  assert(assoc_count(a) == 4);
  assoc_insert(&a, &c[4], NULL);
  assert(assoc_count(a) == 5);
  assoc_insert(&a, &c[5], NULL);
  assert(assoc_count(a) == 6);
  assoc_insert(&a, &c[6], NULL);
  assert(assoc_count(a) == 7);
  assoc_free(a);

  printf("end!\n");
}
