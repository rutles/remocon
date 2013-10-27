#include <remocon.h>

char *rc_getcommand(FILE *file, const char *button){
  char *s;

  fseek(file, 0, SEEK_SET);
  while((s = rc_gettoken(file)) != NULL){
    if(strcmp(s, "button") == 0){
      s = rc_gettoken(file);
      if(strcmp(s, button) == 0){
        s = rc_gettoken(file);
        //return strdup(s);
        return s;
      }
    }
  }
  return NULL;
}
