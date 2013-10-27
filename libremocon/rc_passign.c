// rc_passign.c
// libremocon member function
// Tetsuya Suzuki, 2013

#include <remocon.h>

int rc_passign(int port){
  int ret;
  int fd;
  char buf[40];

  // enable port
  fd = open("/sys/class/gpio/export", O_WRONLY);
  if(fd == -1){
    fprintf(stderr, "fail to open GPIO%d. %s\n", port, strerror(errno));
    return -1;
  }
  sprintf(buf, "%d", port);
  ret = write(fd, buf, strlen(buf));
  close(fd);
  if(ret <= 0){
    fprintf(stderr, "fail to open GPIO%d. %s\n", port, strerror(errno));
    return -1;
  }

  // set port as input
  sprintf(buf, "/sys/class/gpio/gpio%d/direction", port);
  fd = open(buf, O_WRONLY);
  write(fd, "in", 2);
  close(fd);

  // set port value change (edge) detect
  sprintf(buf, "/sys/class/gpio/gpio%d/edge", port);
  fd = open(buf, O_WRONLY);
  write(fd, "both", 4);
  close(fd);

  // open port for read
  sprintf(buf, "/sys/class/gpio/gpio%d/value", port);
  fd = open(buf, O_RDONLY);

  return fd;
}