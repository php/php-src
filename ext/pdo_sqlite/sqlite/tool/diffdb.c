/*
** A utility for printing the differences between two SQLite database files.
*/
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


#define PAGESIZE 1024
static int db1 = -1;
static int db2 = -1;

int main(int argc, char **argv){
  int iPg;
  unsigned char a1[PAGESIZE], a2[PAGESIZE];
  if( argc!=3 ){
    fprintf(stderr,"Usage: %s FILENAME FILENAME\n", argv[0]);
    exit(1);
  }
  db1 = open(argv[1], O_RDONLY);
  if( db1<0 ){
    fprintf(stderr,"%s: can't open %s\n", argv[0], argv[1]);
    exit(1);
  }
  db2 = open(argv[2], O_RDONLY);
  if( db2<0 ){
    fprintf(stderr,"%s: can't open %s\n", argv[0], argv[2]);
    exit(1);
  }
  iPg = 1;
  while( read(db1, a1, PAGESIZE)==PAGESIZE && read(db2,a2,PAGESIZE)==PAGESIZE ){
    if( memcmp(a1,a2,PAGESIZE) ){
      printf("Page %d\n", iPg);
    }
    iPg++;
  }
  printf("%d pages checked\n", iPg-1);
  close(db1);
  close(db2);
}
