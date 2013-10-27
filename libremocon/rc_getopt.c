#include <remocon.h>

int rc_getopt(int argc, char *argv[], struct opspec *op){
  int opt;
  char *gpio;
  int error;
  static struct option long_options[] = {
    {"gpio",      required_argument, NULL, 'g'},
    {"name",      required_argument, NULL, 'n'},
    {"directory", required_argument, NULL, 'd'},
    {"lirc",      no_argument,       NULL, 'l'},
    {0, 0, 0, 0}
  };

  error = 0;
  gpio = NULL;
  op->name = "noname";
  op->dir = (char *)get_current_dir_name();
  op->lirc = 0;

  while((opt = getopt_long(argc, argv, "g:n:d:l", long_options, NULL)) != -1){
    switch(opt){
      case 'g':
        gpio = optarg;
        break;
      case 'n':
        op->name = optarg;
        break;
      case 'd':
        free(op->dir);
        op->dir = optarg;
        break;
      case 'l':
        op->lirc = 1;
        break;
      default:
        error = 1;
        break;
    }
  }
  if(error){
    fprintf(stderr, "unknown option found.\n");
    return -1;
  }

  if(gpio == NULL){
    op->port = 25;
  } else {
    op->port = atoi(gpio);
    if(op->port == 0){
      op->port = 25;
      fprintf(stderr, "GPIO number %s is invalid.\n", gpio);
      fprintf(stderr, "port was set GPIO25.\n");
    }
  }

  return 0;
}
