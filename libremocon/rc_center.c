// rc_center.c
// libremocon member function
// Tetsuya Suzuki, 2013

#include <remocon.h>

// find center value
long rc_center(long *array, int length){
  long temp;
  int i, j;

  for(i = 0; i < length - 1; i++)
    for(j = length - 1; j > i; j--)
      if(array[j - 1] > array[j]){
        temp = array[j];
        array[j] = array[j - 1];
        array[j - 1] = temp;
      }
  return array[length / 2];
}