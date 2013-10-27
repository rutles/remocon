#include <remocon.h>

int rc_memo(int serial, struct rcspec *rc){
  FILE *ftmp;
  char buf[18];

  if(serial > 99){
    fprintf(stderr, "Too many memos.\n");
    return -1;
  }

  sprintf(buf, "/tmp/rcmemo%02d.tmp", serial);
  ftmp = fopen(buf, "wb");
  if(ftmp == NULL){
    fprintf(stderr, "fail to memo. %s.\n", strerror(errno));
    return -1;
  }

  fwrite(&rc->multi, sizeof(int), 1, ftmp);
  fwrite(&rc->count1, sizeof(int), 1, ftmp);
  fwrite(rc->code1, sizeof(unsigned char), rc->count1, ftmp);
  if(rc->multi){
    fwrite(&rc->count2, sizeof(int), 1, ftmp);
    fwrite(rc->code2, sizeof(unsigned char), rc->count2, ftmp);
  }

  fclose(ftmp);
  return 0;
}