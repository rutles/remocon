// rc_next.c
// libremocon member function
// Tetsuya Suzuki, 2013

#include <remocon.h>

// detect reader of next signal
int rc_next(struct irspec *ir){
  char c; // PL-IRM2161-XD1 connected port value
  int ret;// poll return value. for detect timeout or error
  long temp;
  struct timespec now;
  struct pollfd pfd;
  pfd.fd = ir->fd;
  pfd.events = POLLPRI;

  //detect leader off->on
  lseek(ir->fd, 0, SEEK_SET);// prepare read
  ret = poll(&pfd, 1, RC_CONTINUE);// wait for value change until timeout
  clock_gettime(CLOCK_MONOTONIC_RAW, &now);
  read(ir->fd, &c, 1);// dummy read
  if(ret <= 0)// not detect next signal
    return ret;

  ir->gap = RC_LAP(now, ir->origin);// from top of previous signal
  ir->interval = ir->gap - ir->last;// from tail of previous signal

  //pass reader on
  lseek(ir->fd, 0, SEEK_SET);// prepare read
  ret = poll(&pfd, 1, RC_DETECT);// on->off
  clock_gettime(CLOCK_MONOTONIC_RAW, &now);
  ir->last = RC_LAP(now, ir->origin);
  read(ir->fd, &c, 1);// dummy read
  if(ret <= 0)
    return ret;

  //pass leader off
  lseek(ir->fd, 0, SEEK_SET);// prepare read
  ret = poll(&pfd, 1, RC_TIMEOUT);
  clock_gettime(CLOCK_MONOTONIC_RAW, &now);
  read(ir->fd, &c, 1);// dummy read
  temp = RC_LAP(now, ir->origin);
  ir->repeat = temp - ir->last;
  ir->last = temp;
  return ret;
}