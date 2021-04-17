#define INITSIZE 17
#define SCALEFACTOR 2

struct dataframe {
  void* key;
  void* data;
};
typedef struct dataframe dataframe;

struct assoc {
  dataframe* table;
  unsigned int size;
  unsigned int capacity;
  int keysize;
};
typedef struct assoc assoc;
