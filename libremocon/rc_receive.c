// rc_receive.c
// libremocon member function
// Tetsuya Suzuki, 2013

#include <remocon.h>

int rc_receive(int fd, struct rcspec *rc){
  struct irspec ir;
  int ret;

  // setup required parameters
  ir.fd = fd;
  ir.code = rc->code1;
  ret = rc_start(&ir);// wait for first shot
  if(ret <= 0){
    return -1;
  }
  rc_decode(&ir);// record signal
  rc->count1 = ir.count;// save signal length in bits

  ret = rc_next(&ir);// check next shot
  if(ret <= 0){
    rc->multi = 0;
  }
  else
  if((ir.repeat < ir.loff * 8 / 10) ||
  (ir.repeat > ir.loff * 12 / 10)){
    rc->multi = 0;
  } else {
    ir.code = rc->code2;
    rc_decode(&ir);
    rc->multi = 1;
    rc->count2 = ir.count;
  }
  return 0;
}
