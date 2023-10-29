
#include <stdio.h>
#include "my_alloc.h"

int main(int argc, char **argv) {
  int *data, *data2, *d1, *d2;

  data = (int *)my_malloc(100000 * sizeof(int),(char)1);
  data2 = (int *)my_malloc(100000 * sizeof(int),(char)2);
  for (int i = 0; i < 100000; i++) {
    data[i] = i;
    data2[i] = i;
  }
  for (int i = 0; i < 100000; i++) {
    printf("%x: %d\n", &data[i], data[i]);
  }
  for (int i = 0; i < 100000; i++) {
    printf("%x: %d\n", &data2[i], data2[i]);
  }
  my_free(data2);
  my_realloc(data, 150000,(char)15);
  my_free(data);

  d1 = (int *)my_malloc(1000,(char)20);
  d2 = (int *)my_malloc(1000,(char)20);
  my_free(d1);
  my_free(d2);
  return 0;
}