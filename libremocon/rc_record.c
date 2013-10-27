// rc_record.c
// libremocon member function
// Tetsuya Suzuki, 2013

#include <remocon.h>

// record signal
int rc_record(struct irspec *ir){
  int i; //loop counter
  int ret;// poll return value. for detect timeout or error
  char c; // PL-IRM2161-XD1 connected port value
  long temp;
  struct timespec now;
  struct pollfd pfd;
  pfd.fd = ir->fd;
  pfd.events = POLLPRI;

  // signal record
  for(i = 0; i < RC_SIGLEN; i++){
    lseek(ir->fd, 0, SEEK_SET);// prepare read
    poll(&pfd, 1, -1);// on->off
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    read(ir->fd, &c, 1);// dummy read
    temp = RC_LAP(now, ir->origin);
    ir->son[i] = temp - ir->last;
    ir->last = temp;

    lseek(ir->fd, 0, SEEK_SET);// prepare read
    ret = poll(&pfd, 1, RC_TIMEOUT);//off->on
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    read(ir->fd, &c, 1);// dummy read
    temp = RC_LAP(now, ir->origin);
    ir->soff[i] = temp - ir->last;
    ir->last = temp;
    if(ret <= 0){
      ir->length = i;
      return 0;
    }
  }
  return -1;
}