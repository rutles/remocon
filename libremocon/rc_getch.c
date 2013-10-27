#include <remocon.h>

int rc_getch(){
  struct termios old;
  struct termios now;
  int ch;

  tcgetattr( STDIN_FILENO, &old);
  now = old;
  now.c_lflag &= ~(ICANON | ECHO);
  tcsetattr( STDIN_FILENO, TCSANOW, &now);
  ch = getchar();
  tcsetattr( STDIN_FILENO, TCSANOW, &old);

  return ch;
}
