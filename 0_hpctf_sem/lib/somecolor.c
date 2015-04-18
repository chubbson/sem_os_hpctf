#include <somecolor.h>
#include <stdio.h>
#include <string.h>

int scatcolor(int idx, char *str)
{
  const int shftidx = 0x41; // lowest ascii tablechar
  const int alphcnt = 26;
  const int lettercnt = (alphcnt*2);
  const int coldigcnt = 6;
  const int fldcolcnt = coldigcnt*coldigcnt;
  const int plrcnt = coldigcnt // each main uni color,  
                   + (fldcolcnt // each fore and background colorcombination 
                      * lettercnt); // multiply by each character combination

  char buf[30];
  buf [0] = '\0';
  int n = 0;
  int letidx, digbg, digfg;
  idx = idx%plrcnt;
  switch(idx)
  {
    case -1:
      n = sprintf(buf, "\x1B[%d;3%d;4%dm%c\x1B[0m", 0, 7, 7, ' ');
      break;
    case 0:
      n = sprintf(buf, "\x1B[0m%c", ' ');
      break;
    case 1 ... 6:
      n = sprintf(buf,  "\x1B[%d;3%d;4%dm%c\x1B[0m", 0, idx, idx, ' ');
      break;
    case 7 ... 1878:
      idx -= coldigcnt;
      letidx = (idx-1) % lettercnt ;
  //    letidx = (idx-1) / fldcolcnt;
      letidx += letidx >= alphcnt ? 6 : 0;
      letidx += shftidx;


      digfg = ((idx-1) / lettercnt)%coldigcnt + 1;
  //    digfg = ((idx-1)%fldcolcnt)/coldigcnt + 1;// 
      digbg = ((idx-1) % fldcolcnt)%coldigcnt + 1;
      n = sprintf(buf, "\x1B[%d;3%d;4%dm%c\x1B[0m", 1, digfg, digbg, (char)letidx);
      break;
    default:
      break;
  }

  if (n > 0)
  {
    buf[n] = '\0';
    strcat(str, buf);
  }

  //printf("%d %d %d %d\n",letidx, idx, digfg, digbg);

  return n;
}

void printcolor(int idx)
{
  char buf[30];
  buf[0] = '\0';

  scatcolor(idx, buf);
  printf("%s", buf);
}
