#include <remocon.h>

char *rc_gettoken(FILE *file){
  static char buf[80];
  int c;
  int len;

  // skip space
  do{
    if((c = fgetc(file)) == EOF)
      return NULL;
    if(c == '#'){
      do{
        if((c = fgetc(file)) == EOF)
          return NULL;
      } while (c != '\n');
    }
  } while(strchr(" ,=:\t\n", c));

  // get token
  if(c == '"'){
    for(len = 0; len < 79 && c != EOF; len++){
      c = fgetc(file);
      if(c == '"'){
        buf[len] = 0;
        return buf;
      }
      buf[len] = (char)c;
    }
    return NULL;
  }

  len = 0;
  do{
    if(len < 79){
      buf[len++] = (char)c;
    } else {
      return NULL;
    }
    if((c = fgetc(file)) == EOF)
      return NULL;
  } while(isalnum(c));

  buf[len] = 0;
  return buf;
}

