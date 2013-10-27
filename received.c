#include <remocon.h>

int main(int argc, char **argv){
  FILE *file1, *file2;
  int i;
  int ret;
  int count;
  char **button;
  struct opspec op;
  struct rcspec rc1, rc2;
  unsigned char code1[RC_SIGCNT], code2[RC_SIGCNT];
  char buf[80];
  char *command;

  rc_getopt(argc, argv, &op);
  sprintf(buf, "%s/analyzer.conf", op.dir);
  file1 = fopen(buf, "r");
  if(file1 == NULL){
    fprintf(stderr, "fail to open %s. %s\n", buf, strerror(errno));
    return -1;
  }
  sprintf(buf, "%s/received.conf", op.dir);
  file2 = fopen(buf, "r");
  if(file2 == NULL){
    fprintf(stderr, "fail to open %s. %s\n", buf, strerror(errno));
    return -1;
  }

  count = rc_getbuttons(file1, &button);
  rc2.code1 = code1;
  rc2.code2 = code2;
  ret = daemon(0, 0);
  if(ret < 0){
    fprintf(stderr, "daemon can't start.\n");
    return -1;
  }

  while(1){
    ret = rc_analyze(op.port, &rc2);//ir catch
    if(ret < 0)// occur some error during ir catch
      exit(EXIT_FAILURE);

    for(i = 0; i < count; i++){
      ret = rc_getdata(file1, button[i], &rc1);
      if(ret < 0)// button not entry in analyzer.conf
        continue;
      if(rc_speccmp(&rc1, &rc2) == 0){
        command = rc_getcommand(file2, button[i]);
        system(command);//if not entry, no operation
        break;
      }
    }
  }
}
