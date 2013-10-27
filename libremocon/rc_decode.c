// rc_decode.c
// libremocon member function
// Tetsuya Suzuki, 2013

#include <remocon.h>

// decode signal
int rc_decode(struct irspec *ir){
  int i, j; //loop counter
  int ret = 1;// poll return value. for detect timeout or error
  char c; // PL-IRM2161-XD1 connected port value
  long temp;// temporary use in a working
  long son, soff;
  struct timespec now;
  struct pollfd pfd;
  pfd.fd = ir->fd;
  pfd.events = POLLPRI;

  //Signal analyze
  for(i = 0; i < RC_SIGCNT; i++)
    for(j = 0; j < 8; j++){
      lseek(ir->fd, 0, SEEK_SET);// prepare read
      poll(&pfd, 1, -1);// on->off
      clock_gettime(CLOCK_MONOTONIC_RAW, &now);
      read(ir->fd, &c, 1);// dummy read
      temp = RC_LAP(now, ir->origin);
      son = temp - ir->last;
      ir->last = temp;

      lseek(ir->fd, 0, SEEK_SET);// prepare read
      ret = poll(&pfd, 1, RC_TIMEOUT);//off->on
      clock_gettime(CLOCK_MONOTONIC_RAW, &now);
      read(ir->fd, &c, 1);// dummy read
      temp = RC_LAP(now, ir->origin);
      soff = temp - ir->last;
      ir->last = temp;
      ir->code[i] <<= 1;
      ir->code[i] |= (son * 2 < soff);
      if(ret <= 0){
        ir->length = i * 8 + j;
        ir->count = i;
        return 0;
      }
  }
  return -1;
}
