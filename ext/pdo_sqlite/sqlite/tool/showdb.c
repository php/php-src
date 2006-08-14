/*
** A utility for printing all or part of an SQLite database file.
*/
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


static int pagesize = 1024;
static int db = -1;
static int mxPage = 0;
static int perLine = 32;

static void out_of_memory(void){
  fprintf(stderr,"Out of memory...\n");
  exit(1);
}

static print_page(int iPg){
  unsigned char *aData;
  int i, j;
  aData = malloc(pagesize);
  if( aData==0 ) out_of_memory();
  lseek(db, (iPg-1)*pagesize, SEEK_SET);
  read(db, aData, pagesize);
  fprintf(stdout, "Page %d:\n", iPg);
  for(i=0; i<pagesize; i += perLine){
    fprintf(stdout, " %03x: ",i);
    for(j=0; j<perLine; j++){
      fprintf(stdout,"%02x ", aData[i+j]);
    }
    for(j=0; j<perLine; j++){
      fprintf(stdout,"%c", isprint(aData[i+j]) ? aData[i+j] : '.');
    }
    fprintf(stdout,"\n");
  }
  free(aData);
}

int main(int argc, char **argv){
  struct stat sbuf;
  if( argc<2 ){
    fprintf(stderr,"Usage: %s FILENAME ?PAGE? ...\n", argv[0]);
    exit(1);
  }
  db = open(argv[1], O_RDONLY);
  if( db<0 ){
    fprintf(stderr,"%s: can't open %s\n", argv[0], argv[1]);
    exit(1);
  }
  fstat(db, &sbuf);
  mxPage = sbuf.st_size/pagesize + 1;
  if( argc==2 ){
    int i;
    for(i=1; i<=mxPage; i++) print_page(i);
  }else{
    int i;
    for(i=2; i<argc; i++){
      int iStart, iEnd;
      char *zLeft;
      iStart = strtol(argv[i], &zLeft, 0);
      if( zLeft && strcmp(zLeft,"..end")==0 ){
        iEnd = mxPage;
      }else if( zLeft && zLeft[0]=='.' && zLeft[1]=='.' ){
        iEnd = strtol(&zLeft[2], 0, 0);
      }else{
        iEnd = iStart;
      }
      if( iStart<1 || iEnd<iStart || iEnd>mxPage ){
        fprintf(stderr,
          "Page argument should be LOWER?..UPPER?.  Range 1 to %d\n",
          mxPage);
        exit(1);
      }
      while( iStart<=iEnd ){
        print_page(iStart);
        iStart++;
      }
    }
  }
  close(db);
}
