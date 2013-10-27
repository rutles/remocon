/*
  analyzer.c
  description: Ir-Remocon signal analyzer
  target: Raspberry Pi + Raspbian
  hardware: PL-IRM2161-XD1 shall be connected to GPIOxx
  compile: cc analyzer.c -o analyzer -lrt -lremocon
  execute: sudo ./analyzer [-g<xx>] [-n<name>] [-d<directry>] [-l]
*/

#include <remocon.h>

int analyze(int port, struct rcspec *rc){
  int i;
  int ret;

  printf("Analyze start.\n");

  ret = rc_analyze(port, rc);
  if(ret < 0)
    return -1;

  printf("Analyze complete as follows.\n\n");

  //report analyze
  printf("Format: ");
  printf(rc->lon < RC_FORMAT ? "IEHA, " : "NEC, ");
  printf(rc->multi ? "multi shot.\n" : "single shot.\n");
  printf("Leader ON: %dusec, OFF: %dusec.\n", rc->lon, rc->loff);
  printf("Signal T: %dusec, ", rc->t);
  printf("H: %dusec, ", rc->h);
  printf("L: %dusec.\n\n", rc->l);

  if(rc->multi)
    printf("1st shot ");

  printf("Signal count: %dbytes.\n", rc->count1);
  for(i = 0; i < rc->count1 - 1; i++){
    printf("%02X", rc->code1[i]);
    printf((i + 1) % 8 ? ", " : "\n");
  }
  printf("%02X.\n\n", rc->code1[i]);

  if(rc->multi){
    printf("Gap: %dusec.\n", rc->gap);
    printf("Interval: %dusec.\n\n", rc->interval);

    printf("2nd shot ");
    printf("Signal count: %dbytes.\n", rc->count2);
    for(i = 0; i < rc->count2 - 1; i++){
      printf("%02X", rc->code2[i]);
      printf((i + 1) % 8 ? ", " : "\n");
    }
    printf("%02X.\n\n", rc->code2[i]);
  }
  return 0;
}

int main(int argc, char *argv[]){
  struct opspec op;
  struct rcspec rc;
  int serial;
  int ret;
  unsigned char code1[RC_SIGCNT], code2[RC_SIGCNT];

  ret = rc_getopt(argc, argv, &op);
  if(ret < 0)
    return -1;

  rc.code1 = code1;
  rc.code2 = code2;
  serial = 0;

  while(1){
    ret = analyze(op.port, &rc);
    if(ret < 0){
      fprintf(stderr, "Aboat analyze.\n");
      return -1;
    }

    while(1){
      printf("[T]est [M]emo [C]ontinue [Q]uit");
      do{
        ret = tolower(rc_getch());
      } while(strchr("tmcq", ret) == NULL);
      printf("\n\n");

      if(ret == 't'){
        rc_shot(&rc);
        printf("IR signal was shot via GPIO4.\n\n");
        continue;
      }

      if(ret == 'm'){
        rc_memo(serial++, &rc);
        printf("memo done.\n\n");
        continue;
      }

      if(ret == 'c'){
        break;
      }

      if(ret == 'q'){
        if(serial == 0)
          return 0;

        ret = rc_save(serial, &op, &rc);
        if(ret < 0){
          fprintf(stderr, "fail to save.\n");
          return -1;
        }

        if(!op.lirc){
          printf("Create %s/analyzer.conf.\n", op.dir);
          printf("saved in analyzer standard format.\n\n");
        } else {
          printf("Create %s/lirc.conf.\n", op.dir);
          if(rc.multi == 0 || rc.count1 == rc.count2){
            printf("saved in lirc standard format.\n\n");
          } else {
            printf("saved in lirc raw format.\n\n");
          }
        }
        return 0;
      }
    }
  }
}
