#include <remocon.h>

int main(int argc, char **argv){
  FILE *file;
  int i;
  int ret;
  struct opspec op;
  struct rcspec rc;
  char buf[80];
  char *command;

  rc_getopt(argc, argv, &op);
  sprintf(buf, "%s/analyzer.conf", op.dir);
  file = fopen(buf, "r");
  if(file == NULL){
    fprintf(stderr, "fail to open %s. %s\n", buf, strerror(errno));
    return -1;
  }
  rc_getdata(file, "01", &rc);
  printf("Format: ");
  printf(rc.lon < RC_FORMAT ? "IEHA, " : "NEC, ");
  printf(rc.multi ? "multi shot.\n" : "single shot.\n");
  printf("Leader ON: %dusec, OFF: %dusec.\n", rc.lon, rc.loff);
  printf("Signal T: %dusec, ", rc.t);
  printf("H: %dusec, ", rc.h);
  printf("L: %dusec.\n\n", rc.l);

  if(rc.multi)
    printf("1st shot ");

  printf("Signal count: %dbytes.\n", rc.count1);
  for(i = 0; i < rc.count1 - 1; i++){
    printf("%02X", rc.code1[i]);
    printf((i + 1) % 8 ? ", " : "\n");
  }
  printf("%02X.\n\n", rc.code1[i]);

  if(rc.multi){
    printf("Gap: %dusec.\n", rc.gap);
    printf("Interval: %dusec.\n\n", rc.interval);

    printf("2nd shot ");
    printf("Signal count: %dbytes.\n", rc.count2);
    for(i = 0; i < rc.count2 - 1; i++){
      printf("%02X", rc.code2[i]);
      printf((i + 1) % 8 ? ", " : "\n");
    }
    printf("%02X.\n\n", rc.code2[i]);
  }
  return 0;

}
