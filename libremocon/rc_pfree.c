// rc_pfree.c
// libremocon member function
// Tetsuya Suzuki, 2013

#include <remocon.h>

void rc_pfree(int port){
  int fd;
  char buf[40];

  // disable port
  fd = open("/sys/class/gpio/unexport", O_WRONLY);
  sprintf(buf, "%d", port);
  write(fd, buf, strlen(buf));
  close(fd);
}