#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <lm4f120h5qr.h>
#include <uart.h>

#include <system.h>

void print_num(int num);

void term_reset() {
  printf("\ec");fflush(stdout);
}

void term_hide_cursor() {
  printf("\e[?25l");fflush(stdout);
}

void term_set_scrolling(int l_from, int l_to) {
  printf("\e[");print_num(l_from);
  putchar(';');print_num(l_to);
  putchar('r');fflush(stdout);

}

void term_goto(int x, int y) {
  printf("\e[");print_num(y);
  putchar(';');print_num(x);
  putchar('H');fflush(stdout);
}

void term_set_color(char fg, char bg) {
  printf("\e[");print_num(fg+30);
  putchar(';');print_num(bg+40);
  putchar('m');fflush(stdout);
}

void term_hline(int x, int y, int len, char c) {
  term_goto(x,y);
  while (len--) {
    putchar(c);
  }
  fflush(stdout);
}

int FUHA = 0;
void get_terminal_size(int *x, int *y);
void statusbar(char *buf, int x, int y) {
//  int x, y;

  get_terminal_size(&x, &y);
  printf("\e7"); //save cursor
  fflush(stdout);
  term_set_color(7, 1);
  term_hline(1,1,x,' ');
  term_goto(x-10, 1); printf("monitor");

  fflush(stdout);
  term_goto(2, 1); print_num(FUHA);
  fflush(stdout);

  term_goto(15, 1);
  printf("tasks: ");
  fflush(stdout);
  int task_count = 0;
  print_num(task_count);
  fflush(stdout);

  term_goto(26, 1);
  printf((UART0_MIS_R & (1<<4)) ? "ON" : "OFF");
  fflush(stdout);

  term_goto(33, 1);
  printf("'%s'",buf);
  fflush(stdout);

  printf("\e8"); //restore cursor
  fflush(stdout);
  term_set_scrolling(3, y);
  fflush(stdout);
  term_set_color(9,9);
  term_goto(1, y);
}

void monitor_main(void) {

  FUHA = 0;
  term_reset();
  int x=60,y=15;
  get_terminal_size(&x, &y);
  statusbar("",x,y);

  term_hide_cursor();
  term_goto(2,2);
  printf("\e[?67h");fflush(stdout);

/*
  int x, y;
  get_terminal_size(&x, &y);
  for (int i=0; i<y*2; i++) {
    print_num(i);
    printf("\r\n"); fflush(stdout);
  }*/

  //asm volatile ("svc #0");
  char buf[16]="\0"; int bufc = 0;
  while(1){
    int c = -1;

    if (uart_notempty(0)) {
      c = uart_getc(0);
      switch (c) {
        case '\n':
        case '\r':
          if (strncmp("help", buf, 4) == 0) {
            printf("commands:\nhelp\nstats\n");
          } else if (strncmp("stats", buf, 5) == 0) {
            int cnt = syscall_count();
            printf("syscalls: ");print_num(cnt);putchar('\n');
            fflush(stdout);
            printf("uart tx/rx: ");print_num(uart_b_tx);
            putchar('/');print_num(uart_b_rx);
            putchar('\n');
          } else {
            printf("unknown cmd: '%s'\n", buf);
          }
          fflush(stdout);

          memset(buf, 0, 16);
          bufc = 0;
          break;
        case '\b':
        case 0x7f:
          if (!bufc)
            break;
          buf[--bufc] = 0;
          break;
        default:
          if (bufc<15) {
            buf[bufc++] = c;
          }
          break;
      }
    }
    if (FUHA % 0xc == 0)
      statusbar(buf, x, y);
    FUHA++;
    asm volatile("wfi");
  };
  return ;

}

// readupto
// read max len chars and stop at first separator occurence
void readupto(char *buf, int *cnt, char sep, int len) {
  char *_c = buf;
  do {
    read(0, _c, 1);
  }while((*(_c++) != sep) && ((_c-buf) < len));
  *cnt = _c - buf;
}

void print_num(int num) {
  char p[16]; int i = 0;
  p[0] = '0';
  do {
    int x = num - 10*(num/10);
    num /= 10;
    p[i++] = (x+'0');
  } while (num!=0);
//  i = !i? 1: 0;
  while (i--) {
    putchar(p[i]);
  }
}

void get_terminal_size(int *x, int *y) {
  printf("\e[18t");fflush(stdout);
  char buf[10];
  int cnt;
  // response is like \033[8;29;105t
  readupto(buf, &cnt, ';', 10); //skip first part
  readupto(buf, &cnt, ';', 10); //load X into buf
  int r = 0;
  int i = 0;
  for (i = 0; i < 10; i++) {
    if (buf[i] == ';') break;
    r *= 10;
    r += buf[i] - '0';
  }
  *y = r;
  readupto(buf, &cnt, 't', 10); //load Y into buf
  r = 0;
  for (i = 0; i < 10; i++) {
    if (buf[i] == 't') break;
    r *= 10;
    r += buf[i] - '0';
  }
  *x = r;

}

