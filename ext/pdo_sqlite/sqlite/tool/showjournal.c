/*
** A utility for printing an SQLite database journal.
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

static void out_of_memory(void){
  fprintf(stderr,"Out of memory...\n");
  exit(1);
}

static print_page(int iPg){
  unsigned char *aData;
  int i, j;
  aData = malloc(pagesize);
  if( aData==0 ) out_of_memory();
  read(db, aData, pagesize);
  fprintf(stdout, "Page %d:\n", iPg);
  for(i=0; i<pagesize; i += 16){
    fprintf(stdout, " %03x: ",i);
    for(j=0; j<16; j++){
      fprintf(stdout,"%02x ", aData[i+j]);
    }
    for(j=0; j<16; j++){
      fprintf(stdout,"%c", isprint(aData[i+j]) ? aData[i+j] : '.');
    }
    fprintf(stdout,"\n");
  }
  free(aData);
}

int main(int argc, char **argv){
  struct stat sbuf;
  unsigned int u;
  int rc;
  unsigned char zBuf[10];
  unsigned char zBuf2[sizeof(u)];
  if( argc!=2 ){
    fprintf(stderr,"Usage: %s FILENAME\n", argv[0]);
    exit(1);
  }
  db = open(argv[1], O_RDONLY);
  if( db<0 ){
    fprintf(stderr,"%s: can't open %s\n", argv[0], argv[1]);
    exit(1);
  }
  read(db, zBuf, 8);
  if( zBuf[7]==0xd6 ){
    read(db, &u, sizeof(u));
    printf("Records in Journal: %u\n", u);
    read(db, &u, sizeof(u));
    printf("Magic Number: 0x%08x\n", u);
  }
  read(db, zBuf2, sizeof(zBuf2));
  u = zBuf2[0]<<24 | zBuf2[1]<<16 | zBuf2[2]<<8 | zBuf2[3];
  printf("Database Size: %u\n", u);
  while( read(db, zBuf2, sizeof(zBuf2))==sizeof(zBuf2) ){
    u = zBuf2[0]<<24 | zBuf2[1]<<16 | zBuf2[2]<<8 | zBuf2[3];
    print_page(u);
    if( zBuf[7]==0xd6 ){
      read(db, &u, sizeof(u));
      printf("Checksum: 0x%08x\n", u);
    }
  }
  close(db);
}
