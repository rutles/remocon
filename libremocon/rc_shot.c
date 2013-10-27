#include <remocon.h>
#include <sys/mman.h>

// offset address
#define OFF_PRT 0x20200000 // port registers
#define OFF_CLK 0x20101000 // clock registers

// function select value
#define FNC_CLK 4 // GPIO4 only

// Create new mapping of registers
// offset: offset address of registers
// return: pointer to the mapped area, or MAP_FAILED on error
uint32_t *regs_map(off_t offset){
  int fd;
  void *map;

  fd = open("/dev/mem", O_RDWR | O_SYNC);
  if(fd < 0)
    return (uint32_t *)MAP_FAILED;

  map = mmap(NULL,
    getpagesize(),
    PROT_READ | PROT_WRITE,
    MAP_SHARED,
    fd,
    offset);

  close(fd);
  return (uint32_t *)map;
}

// function select
// *regs: GPIO mapping, port: GPIO number, val: function value
void fnc_wr(uint32_t *regs, int port, int val){
  regs[port / 10] &=  ~(7 << ((port % 10) * 3));
  regs[port / 10] |= (val << ((port % 10) * 3));
}

// CLK turn on/off
// *regs: PWM mapping, val: 0 off, 1 on
void clk_sw(uint32_t *regs, int val){
  if(val){
    regs[28] = 0x5A000000 | 0x11;// enable clock
  } else {
    regs[28] = 0x5A000000 | 0x01;// disable clock
    while(regs[28] & 0x80); // loop while busy
  }
}

// set CLK frequency
// *regs: CLK mapping, val: frequency
void clk_frq(uint32_t *regs, uint32_t val){
  uint32_t save;
  int di, dr, df;

  di = 19200000 / val;
  dr = 19200000 % val;
  df = (int)((double)dr * 4096.0 / 19200000.0);

  save = regs[28];//backup on/off
  clk_sw(regs, 0);//temporary off
  regs[29] = 0x5A000000 | (di << 12) | df;
  regs[28] = save;//restore on/off
}

// set GPIO4 as free running clock out
// return: pointer to the mapped area, or MAP_FAILED on error
uint32_t *fnc_clk(){
  uint32_t *regs;

  regs = regs_map(OFF_PRT);
  if(regs == MAP_FAILED)
    return (uint32_t *)MAP_FAILED;
  fnc_wr(regs, 4, FNC_CLK);//set GPIO4 for CLK
  munmap((void *)regs, getpagesize());

  regs = regs_map(OFF_CLK);
  if(regs == MAP_FAILED)
    return (uint32_t *)MAP_FAILED;
  return regs;
}

int rc_shot(struct rcspec *rc){
  int i, j;// loop counter
  int ret;// success flag
  uint32_t *regs;// register map
  uint8_t code;// a byte of ir code

  struct timespec org;
  #define RC_NSLEEP(u) \
  org.tv_nsec += (u * 1000); \
  if(org.tv_nsec >= 1000000000){ \
    org.tv_nsec -= 1000000000; \
    org.tv_sec += 1; \
  } \
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &org, NULL);

  regs = fnc_clk();
  if(regs == MAP_FAILED){
    fprintf(stderr, "fail to get registers map. %s\n", strerror(errno));
    return -1;
  }
  clk_frq(regs, 38000);// 38kHz
  clk_sw(regs, 0);// stop
  clock_gettime(CLOCK_MONOTONIC, &org);

  clk_sw(regs, 1);// leader on
  RC_NSLEEP(rc->lon);
  clk_sw(regs, 0);// leader off
  RC_NSLEEP(rc->loff);

  for(i = 0; i < rc->count1; i++){
    code = rc->code1[i];
    for(j = 0; j < 8; j++){
      clk_sw(regs, 1);// signal on
      RC_NSLEEP(rc->t);
      clk_sw(regs, 0);// signal off
      if(code & 0x80){
        RC_NSLEEP(rc->h);
      } else {
        RC_NSLEEP(rc->l);
      }
      code <<= 1;
    }
  }
  clk_sw(regs, 1);// signal on
  RC_NSLEEP(rc->t);
  clk_sw(regs, 0);// signal off

  if(!rc->multi){
    ret = munmap((void *)regs, getpagesize());
    return ret;
  }

  RC_NSLEEP(rc->interval);// interval

  clk_sw(regs, 1);// leader on
  RC_NSLEEP(rc->lon);
  clk_sw(regs, 0);// leader off
  RC_NSLEEP(rc->loff);

  for(i = 0; i < rc->count2; i++){
    code = rc->code2[i];
    for(j = 0; j < 8; j++){
      clk_sw(regs, 1);// signal on
      RC_NSLEEP(rc->t);
      clk_sw(regs, 0);// signal off
      if(code & 0x80){
        RC_NSLEEP(rc->h);
      } else {
        RC_NSLEEP(rc->l);
      }
      code <<= 1;
    }
  }
  clk_sw(regs, 1);// signal on
  RC_NSLEEP(rc->t);
  clk_sw(regs, 0);// signal off

  ret = munmap((void *)regs, getpagesize());
  return ret;
}
