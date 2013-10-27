#include <remocon.h>

int rc_speccmp(struct rcspec *rc1, struct rcspec *rc2){
  int i;// loop counter
  int neq;// not equal flag

  if(rc1->count1 != rc2->count1)
    return -1;
  neq = 0;
  for(i = 0; i < rc1->count1; i++)
    if(rc1->code1[i] != rc2->code1[i])
      neq = 1;
  if(neq)
    return -1;
  if(rc1->multi != rc2->multi)
    return -1;
  if(rc1->multi){
    if(rc1->count2 != rc2->count2)
      return -1;
    neq = 0;
    for(i = 0; i < rc1->count1; i++)
      if(rc1->code1[i] != rc2->code1[i])
        neq = 1;
    if(neq)
      return -1;
  }
  return 0;
}
