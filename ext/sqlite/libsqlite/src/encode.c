/*
** 2002 April 25
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains helper routines used to translate binary data into
** a null-terminated string (suitable for use in SQLite) and back again.
** These are convenience routines for use by people who want to store binary
** data in an SQLite database.  The code in this file is not used by any other
** part of the SQLite library.
**
** $Id$
*/
#include <string.h>

/*
** Encode a binary buffer "in" of size n bytes so that it contains
** no instances of characters '\'' or '\000'.  The output is 
** null-terminated and can be used as a string value in an INSERT
** or UPDATE statement.  Use sqlite_decode_binary() to convert the
** string back into its original binary.
**
** The result is written into a preallocated output buffer "out".
** "out" must be able to hold at least (256*n + 1262)/253 bytes.
** In other words, the output will be expanded by as much as 3
** bytes for every 253 bytes of input plus 2 bytes of fixed overhead.
** (This is approximately 2 + 1.019*n or about a 2% size increase.)
**
** The return value is the number of characters in the encoded
** string, excluding the "\000" terminator.
*/
int sqlite_encode_binary(const unsigned char *in, int n, unsigned char *out){
  int i, j, e, m;
  int cnt[256];
  if( n<=0 ){
    out[0] = 'x';
    out[1] = 0;
    return 1;
  }
  memset(cnt, 0, sizeof(cnt));
  for(i=n-1; i>=0; i--){ cnt[in[i]]++; }
  m = n;
  for(i=1; i<256; i++){
    int sum;
    if( i=='\'' ) continue;
    sum = cnt[i] + cnt[(i+1)&0xff] + cnt[(i+'\'')&0xff];
    if( sum<m ){
      m = sum;
      e = i;
      if( m==0 ) break;
    }
  }
  out[0] = e;
  j = 1;
  for(i=0; i<n; i++){
    int c = (in[i] - e)&0xff;
    if( c==0 ){
      out[j++] = 1;
      out[j++] = 1;
    }else if( c==1 ){
      out[j++] = 1;
      out[j++] = 2;
    }else if( c=='\'' ){
      out[j++] = 1;
      out[j++] = 3;
    }else{
      out[j++] = c;
    }
  }
  out[j] = 0;
  return j;
}

/*
** Decode the string "in" into binary data and write it into "out".
** This routine reverses the encoded created by sqlite_encode_binary().
** The output will always be a few bytes less than the input.  The number
** of bytes of output is returned.  If the input is not a well-formed
** encoding, -1 is returned.
**
** The "in" and "out" parameters may point to the same buffer in order
** to decode a string in place.
*/
int sqlite_decode_binary(const unsigned char *in, unsigned char *out){
  int i, c, e;
  e = *(in++);
  i = 0;
  while( (c = *(in++))!=0 ){
    if( c==1 ){
      c = *(in++);
      if( c==1 ){
        c = 0;
      }else if( c==2 ){
        c = 1;
      }else if( c==3 ){
        c = '\'';
      }else{
        return -1;
      }
    }
    out[i++] = (c + e)&0xff;
  }
  return i;
}

#ifdef ENCODER_TEST
/*
** The subroutines above are not tested by the usual test suite.  To test
** these routines, compile just this one file with a -DENCODER_TEST=1 option
** and run the result.
*/
int main(int argc, char **argv){
  int i, j, n, m, nOut;
  unsigned char in[30000];
  unsigned char out[33000];

  for(i=0; i<sizeof(in); i++){
    printf("Test %d: ", i+1);
    n = rand() % (i+1);
    if( i%100==0 ){
      int k;
      for(j=k=0; j<n; j++){
        /* if( k==0 || k=='\'' ) k++; */
        in[j] = k;
        k = (k+1)&0xff;
      }
    }else{
      for(j=0; j<n; j++) in[j] = rand() & 0xff;
    }
    nOut = sqlite_encode_binary(in, n, out);
    if( nOut!=strlen(out) ){
      printf(" ERROR return value is %d instead of %d\n", nOut, strlen(out));
      exit(1);
    }
    m = (256*n + 1262)/253;
    printf("size %d->%d (max %d)", n, strlen(out)+1, m);
    if( strlen(out)+1>m ){
      printf(" ERROR output too big\n");
      exit(1);
    }
    for(j=0; out[j]; j++){
      if( out[j]=='\'' ){
        printf(" ERROR contains (')\n");
        exit(1);
      }
    }
    j = sqlite_decode_binary(out, out);
    if( j!=n ){
      printf(" ERROR decode size %d\n", j);
      exit(1);
    }
    if( memcmp(in, out, n)!=0 ){
      printf(" ERROR decode mismatch\n");
      exit(1);
    }
    printf(" OK\n");
  }
}
#endif /* ENCODER_TEST */
