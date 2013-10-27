#include <remocon.h>

int rc_getdata(FILE *file, const char *button, struct rcspec *rc){
  char *s;
  int i;
  int dummy_multi;
  int dummy_count;

  rc->lon = rc->loff = 0;
  rc->t = rc->h = rc-> l = 0;
  rc->multi = -1;
  rc->interval = 0;
  rc->count1 = rc->count2 = 0;
  rc->interval = 0;

  fseek(file, 0, SEEK_SET);
  while((s = rc_gettoken(file)) != NULL){
    if(strcmp(s, "leader") == 0){
      rc->lon = atoi(rc_gettoken(file));
      rc->loff = atoi(rc_gettoken(file));
    } else
    if(strcmp(s, "signal") == 0){
      rc->t = atoi(rc_gettoken(file));
      rc->h = atoi(rc_gettoken(file));
      rc->l = atoi(rc_gettoken(file));
    } else
    if(strcmp(s, "interval") == 0){
      rc->interval = atoi(rc_gettoken(file));
    } else
    if(strcmp(s, "button") == 0){
      if(strcmp(button, rc_gettoken(file)) == 0){
        rc_gettoken(file);// skip "multi"
        rc->multi = (strcmp(rc_gettoken(file), "true") == 0) ? 1 : 0;
        rc_gettoken(file);// skip "count" or "count1"
        rc->count1 = atoi(rc_gettoken(file));
        rc->code1 = (unsigned char *)calloc(rc->count1, sizeof(unsigned char));
        for(i = 0; i < rc->count1; i++)
          rc->code1[i] = (unsigned char)strtol(rc_gettoken(file), NULL, 16);
        if(rc->multi){
          rc_gettoken(file);// skip "count2"
          rc->count2 = atoi(rc_gettoken(file));
          rc->code2 = (unsigned char *)calloc(rc->count2, sizeof(unsigned char));
          for(i = 0; i < rc->count2; i++)
           rc->code2[i] = (unsigned char)strtol(rc_gettoken(file), NULL, 16);
        }
      } else {
        rc_gettoken(file); // skip "multi"
        dummy_multi = (strcmp(rc_gettoken(file), "true") == 0) ? 1 : 0;
        rc_gettoken(file); // skip "count" or "count1"
        dummy_count = atoi(rc_gettoken(file));
        for(i = 0; i < dummy_count; i++)
          rc_gettoken(file);
        if(dummy_multi){
          rc_gettoken(file);// skip "count2"
          dummy_count = atoi(rc_gettoken(file));
          for(i = 0; i < dummy_count; i++)
            rc_gettoken(file);
        }
      }
    }
  }

  if(
  rc->lon == 0 || rc->loff == 0 ||
  rc->t == 0 || rc->h == 0 || rc-> l == 0 ||
  rc->multi == -1 ||
  rc->count1 == 0 ||
  (rc->multi == 1 && rc->count2 == 0) ||
  (rc->multi == 1 && rc->interval == 0))
    return -1;

  return 0;
}
