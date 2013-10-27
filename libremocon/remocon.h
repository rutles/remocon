/*
  remocon.h
  description: Ir-Remocon signal analyzer
*/

#ifndef REMOCON_H
#define REMOCON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>
#include <errno.h>
#include <getopt.h>
#include <termios.h>
#include <unistd.h>

#define RC_DETECT 2000 // minimum time in micro second regarded as leader
#define RC_TIMEOUT 10 // timeout in miri second
#define RC_CONTINUE 100 // the time of wait for next signal in miri second
#define RC_FORMAT 6500 // leader on time in micro second regarded as AEHA
#define RC_SIGLEN 512 // maximum signal length in bits
#define RC_SIGCNT ((RC_SIGLEN + 7) / 8)// // maximum signal length in bytes
// time between o->n in usec
#define RC_LAP(n, o) \
  ((n.tv_sec - o.tv_sec) * 1000000 + \
  (n.tv_nsec - o.tv_nsec) / 1000)

// parameters for IR analyze (time unit in usec)
struct irspec{
  struct timespec origin;// signal start time
  int fd;// file descrioter of PL-IRM2161-XD1 connected port
  long lon, loff;// time in usec of reader
  long repeat;// time in usec of repeat reader off
  int length;// data length in bit
  long *son, *soff;// array time of bit stream
  int count;// data length in byte
  unsigned char *code;// array byte code
  long last;// lap time
  long interval;// from tail of 1st to top of 2nd time
  long gap;// from top of 1st to top of 2nd time
};

// parameters for IR analyze (time unit in usec)
struct rcspec{
  long lon, loff;// reader on / off
  long repeat;// reader off when signal is repeat
  long t;// mark
  long h;// high space
  long l;// low space
  int multi;// multi shot flag
  long gap;// from top of previuos signal
  long interval;// from tail of previous signal
  int count1, count2;// signal count in bytes
  unsigned char *code1, *code2;// array byte code
};

struct opspec{
  int port;// PL-IRM2161-XD1 connected port
  const char *name;// Brand name of remote controller
  char *dir;// file save directory
  int lirc;// lirc conpatible flag
};

// detect reader of first signal
int rc_start(struct irspec *ir);

// detect reader of next signal
int rc_next(struct irspec *ir);

// record signal
int rc_record(struct irspec *ir);

// decode signal
int rc_decode(struct irspec *ir);

int rc_passign(int port);

void rc_pfree(int port);

// find center value
long rc_center(long *array, int length);

int rc_analyze(int port, struct rcspec *rc);

int rc_receive(int fd, struct rcspec *rc);

int rc_getopt(int argc, char **argv, struct opspec *op);
int rc_getch(void);
int rc_memo(int serial, struct rcspec *rc);
int rc_save(int serial, struct opspec *op, struct rcspec *rc);
char *rc_gettoken(FILE *file);
int rc_getdata(FILE *file, const char *button, struct rcspec *rc);
int rc_speccmp(struct rcspec *rc1, struct rcspec *rc2);
int rc_shot(struct rcspec *rc);
char *rc_getcommand(FILE *file, const char *button);
int rc_getbuttons(FILE *file, char ***pbutton);
#endif
