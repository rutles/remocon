// rc_start.c
// libremocon member function
// Tetsuya Suzuki, 2013

#include <remocon.h>

// detect reader of first signal
int rc_start(struct irspec *ir){
  char c; // PL-IRM2161-XD1 connected port value
  int ret;// poll return value. for detect timeout or error
  struct timespec now;// clock_gettime require struct
  struct pollfd pfd;// poll require struct
  pfd.fd = ir->fd;  // set fd from parameters
  pfd.events = POLLPRI; // detect value change

  //detect start
  do{
    //detect leader off->on
    do{
      lseek(ir->fd, 0, SEEK_SET);// prepare read
      poll(&pfd, 1, -1);// wait for value change forever
      clock_gettime(CLOCK_MONOTONIC_RAW, &ir->origin);// record start time
      read(ir->fd, &c, 1);// read
    } while (c == '1');// if on->off, try again

    //count leader on length
    lseek(ir->fd, 0, SEEK_SET);// prepare read
    poll(&pfd, 1, -1);// wait for on->off forever
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    read(ir->fd, &c, 1);// dummy read
    ir->lon = RC_LAP(now, ir->origin);// record reader on length
  } while(ir->lon < RC_DETECT);// if noise, try again

  //Count leader off length
  lseek(ir->fd, 0, SEEK_SET);// prepare read
  ret = poll(&pfd, 1, RC_TIMEOUT);// wait for signal until timeout
  clock_gettime(CLOCK_MONOTONIC_RAW, &now);
  read(ir->fd, &c, 1);// dummy read
  ir->last = RC_LAP(now, ir->origin);// record lap time
  ir->loff = ir->last - ir->lon;// record leader off length
  return ret;
}