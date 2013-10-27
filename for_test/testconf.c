#include <remocon.h>

int main(int argc, char **argv){
  int ret;
  int i;
  struct rcspec rc;
//  unsigned char code1[RC_SIGCNT], code2[RC_SIGCNT];
//  rc.code1 = code1;
//  rc.code2 = code2;

  ret = rc_getdata(argv[1], &rc);

  if(ret < 0){
    return -1;
  } else {
 //report analyze
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
      printf("Interval: %dusec.\n\n", rc.interval);

      printf("2nd shot ");
      printf("Signal count: %dbytes.\n", rc.count2);
      for(i = 0; i < rc.count2 - 1; i++){
        printf("%02X", rc.code2[i]);
        printf((i + 1) % 8 ? ", " : "\n");
      }
      printf("%02X.\n\n", rc.code2[i]);
    }
  }
}
