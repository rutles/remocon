// rc_analyze.c
// libremocon member function
// Tetsuya Suzuki, 2013

#include <remocon.h>

int rc_analyze(int port, struct rcspec *rc){
  struct irspec ir;
  int i;
  int ret;
  int length;
  int index;
  long son[RC_SIGLEN], soff[RC_SIGLEN];
  long *work;

  // setup required parameters
  ir.fd = rc_passign(port);
  if(ir.fd <= 0){
    fprintf(stderr, "Port invalid.\n");
    return -1;
  }
  ir.son = son;
  ir.soff = soff;

  ret = rc_start(&ir);// wait for first shot
  if(ret <= 0){
    fprintf(stderr, "Illegal signal format.\n");
    close(ir.fd);
    rc_pfree(port);
    return -1;
  }
  rc_record(&ir);// record signal
  length = ir.length;// save signal length in bits

  ret = rc_next(&ir);// check next shot
  if(ret <= 0){
    rc->multi = 0;
  }
  else
  if((ir.repeat < ir.loff * 8 / 10) ||
  (ir.repeat > ir.loff * 12 / 10)){
    fprintf(stderr, "Repeat code was found. 2nd signal was reduced.\n"
    "Leader off: %dusec, standard: %dusec.\n", ir.repeat, ir.loff);
    rc->multi = 0;
  } else {
    ir.code = rc->code2;
    rc_decode(&ir);
    rc->multi = 1;
  }

  close(ir.fd);
  rc_pfree(port);

  //report analyze
  rc->lon = ir.lon;
  rc->loff = ir.loff;

  // analyze T
  work = (long *)calloc(length, sizeof(long));
  for(i = 0; i < length; i++)
    work[i] = son[i];
  rc->t = rc_center(work, length);

  // analyze H length
  index = 0;
  for(i = 0; i < length; i++){
    if(rc->t * 2 < soff[i]){
      work[index++] = soff[i];
    }
  }
  rc->h = rc_center(work, index);

 // analyze L length
  index = 0;
  for(i = 0; i < length; i++){
    if(rc->t * 2 >= soff[i]){
      work[index++] = soff[i];
    }
  }
  rc->l = rc_center(work, index);

  free(work);

  // decode signal
  for(i = 0; i < length; i++){
    rc->code1[i / 8] <<= 1;
    rc->code1[i / 8] |= (rc->t * 2 < soff[i]);
  }

  rc->count1 = (length + 7) / 8;
  if(length % 8)
    fprintf(stderr, "1st shot is not filled with bit\n");

  if(rc->multi){
    rc->interval = ir.interval;
    rc->gap = ir.gap;
    rc->count2 = ir.count;
    if(ir.length % 8)
      fprintf(stderr, "2nd shot is not filled with bit\n");
  }
  return 0;
}
