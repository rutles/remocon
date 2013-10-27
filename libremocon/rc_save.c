#include <remocon.h>

int rc_save(int serial, struct opspec *op, struct rcspec *rc){
  FILE *file, *ftmp;
  char buf[80];
  int i, j, k;
  unsigned char code;

  // check memo file exists
  ftmp = fopen("/tmp/rcmemo00.tmp", "rb");
  if(ftmp == NULL)
    return 0;
  fclose(ftmp);

  // analyzer format
  if(!op->lirc){
    sprintf(buf, "%s/analyzer.conf", op->dir);
    file = fopen(buf, "w");
    if(file == NULL){
      fprintf(stderr, "fail to create %s. %s.\n", buf, strerror(errno));
      return -1;
    }

    fprintf(file, "# This file was automatically generated\n");
    fprintf(file, "# brand name: %s\n\n", op->name);
    fprintf(file, "leader = %d : %d\n", rc->lon, rc->loff);
    fprintf(file, "signal = %d : %d : %d\n", rc->t, rc->h, rc->l);
    fprintf(file, "interval = %d\n\n", rc->multi ? rc->interval : 20000);

    for(i = 0; i < serial; i++){
      sprintf(buf, "/tmp/rcmemo%02d.tmp", i);
      ftmp = fopen(buf, "rb");
      if(ftmp == NULL){
        fprintf(stderr, "fail to open %s. %s.\n", buf, strerror(errno));
        return -1;
      }
      fread(&rc->multi, sizeof(int), 1, ftmp);
      fread(&rc->count1, sizeof(int), 1, ftmp);
      fread(rc->code1, sizeof(unsigned char), rc->count1, ftmp);
      if(rc->multi){
        fread(&rc->count2, sizeof(int), 1, ftmp);
        fread(rc->code2, sizeof(unsigned char), rc->count2, ftmp);
      }
      fclose(ftmp);
      remove(buf);

      fprintf(file, "button = %02d\n", i);
      fprintf(file, "multi = %s\n", rc->multi ? "true" : "false");
      fprintf(file, "count1 = %d", rc->count1);
      for(j = 0; j < rc->count1; j++){
        if((j % 8) == 0) fprintf(file, "\n");
        fprintf(file, "%02X", rc->code1[j]);
        fprintf(file, (j < rc->count1 - 1) ? ", " : "\n");
      }

      if(rc->multi){
        fprintf(file, "count2 = %d", rc->count2);
        for(j = 0; j < rc->count2; j++){
          if((j % 8) == 0) fprintf(file, "\n");
          fprintf(file, "%02X", rc->code2[j]);
          fprintf(file, (j < rc->count2 - 1) ? ", " : "\n");
        }
      }
      fprintf(file, "\n");
    }
    fclose(file);
    return 0;
  }

  // lirc compatible
  sprintf(buf, "%s/lircd.conf", op->dir);
  file = fopen(buf, "w");
  if(file == NULL){
    fprintf(stderr, "fail to create %s. %s.\n", buf, strerror(errno));
    return -1;
  }
  fprintf(file, "# This file was automatically generated\n\n");
  fprintf(file, "begin remote\n");
  fprintf(file, "  name %s\n", op->name);

  // lirc standard format
  if(rc->multi == 0 || rc->count1 == rc->count2){
    fprintf(file, "  bits %d\n", rc->count1 * 8);
    fprintf(file, "  flags SPACE_ENC|CONST_LENGTH\n");
    fprintf(file, "  eps 30\n");
    fprintf(file, "  aeps 100\n");
    fprintf(file, "  header %6d %6d\n", rc->lon, rc->loff);
    fprintf(file, "  one    %6d %6d\n", rc->t, rc->h);
    fprintf(file, "  zero   %6d %6d\n", rc->t, rc->l);
    fprintf(file, "  ptrail %6d\n", rc->t);
    fprintf(file, "  gap    %6d\n",
      rc->multi ? rc->gap :
      rc->lon + rc->loff * 2 + (rc->t + rc->h) * rc->count1 * 8);
    fprintf(file, "  toggle_bit_mask 0x0\n");
    fprintf(file, "    begin codes\n");

    for(i = 0; i < serial; i++){
      sprintf(buf, "/tmp/rcmemo%02d.tmp", i);
      ftmp = fopen(buf, "rb");
      if(ftmp == NULL){
        fprintf(stderr, "fail to open %s. %s.\n", buf, strerror(errno));
        return -1;
      }
      fread(&rc->multi, sizeof(int), 1, ftmp);
      fread(&rc->count1, sizeof(int), 1, ftmp);
      fread(rc->code1, sizeof(unsigned char), rc->count1, ftmp);
      if(rc->multi){
        fread(&rc->count2, sizeof(int), 1, ftmp);
        fread(rc->code2, sizeof(unsigned char), rc->count2, ftmp);
      }
      fclose(ftmp);
      remove(buf);

      fprintf(file, "      %02d 0x", i);
      for(j = 0; j < rc->count1; j++){
        fprintf(file, "%02X", rc->code1[j]);
      }

      if(rc->multi){
        fprintf(file, " 0x");
        for(j = 0; j < rc->count2; j++){
          fprintf(file, "%02X", rc->code2[j]);
        }
      }
      fprintf(file, "\n");
    }
    fprintf(file, "    end codes\n");
    fprintf(file, "end remote\n");
    fclose(file);
    return 0;
  }//endif(rc->multi == 0 || rc->count1 == rc->count2)

  // lirc raw format
  fprintf(file, "  flags RAW_CODES\n");
  fprintf(file, "  eps 30\n");
  fprintf(file, "  aeps 100\n");
  fprintf(file, "  gap    %6d\n",
    rc->multi ? rc->gap :
    rc->lon + rc->loff * 2 + (rc->t + rc->h) * rc->count1 * 8);
  fprintf(file, "  begin raw_codes\n");

  for(i = 0; i < serial; i++){
    sprintf(buf, "/tmp/rcmemo%02d.tmp", i);
    ftmp = fopen(buf, "rb");
    if(ftmp == NULL){
      fprintf(stderr, "fail to open %s. %s.\n", buf, strerror(errno));
      return -1;
    }
    fread(&rc->multi, sizeof(int), 1, ftmp);
    fread(&rc->count1, sizeof(int), 1, ftmp);
    fread(rc->code1, sizeof(unsigned char), rc->count1, ftmp);
    if(rc->multi){
      fread(&rc->count2, sizeof(int), 1, ftmp);
      fread(rc->code2, sizeof(unsigned char), rc->count2, ftmp);
    }
    fclose(ftmp);
    remove(buf);

    fprintf(file, "    name %02d\n", i);
    fprintf(file, "    %6d%6d", rc->lon, rc->loff);
    for(j = 0; j < rc->count1; j++){
      code = rc->code1[j];
      for(k = 0; k < 8; k++){
        if((k % 4) == 0) fprintf(file, "\n    ");
        fprintf(file, "%6d", rc->t);
        fprintf(file, "%6d", code & 0x80 ? rc->h: rc->l);
        code <<= 1;
      }
    }
    fprintf(file, "\n");

    if(rc->multi){
      fprintf(file, "    %6d\n", rc->t);
      fprintf(file, "    %6d\n", rc->interval + RC_TIMEOUT);
      fprintf(file, "    %6d%6d", rc->lon, rc->loff);

      for(j = 0; j < rc->count2; j++){
        code = rc->code2[j];
        for(k = 0; k < 8; k++){
          if((k % 4) == 0) fprintf(file, "\n    ");
          fprintf(file, "%6d", rc->t);
          fprintf(file, "%6d", code & 0x80 ? rc->h: rc->l);
          code <<= 1;
        }
      }
      fprintf(file, "\n");
    }
    fprintf(file, "    %6d\n", rc->t);
  }
  fprintf(file, "  end raw_codes\n");
  fprintf(file, "end remote\n");
  fclose(file);
  return 0;
}
