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
#include <assert.h>

/*
** How This Encoder Works
**
** The output is allowed to contain any character except 0x27 (') and
** 0x00.  This is accomplished by using an escape character to encode
** 0x27 and 0x00 as a two-byte sequence.  The escape character is always
** 0x01.  An 0x00 is encoded as the two byte sequence 0x01 0x01.  The
** 0x27 character is encoded as the two byte sequence 0x01 0x28.  Finally,
** the escape character itself is encoded as the two-character sequence
** 0x01 0x02.
**
** To summarize, the encoder works by using an escape sequences as follows:
**
**       0x00  ->  0x01 0x01
**       0x01  ->  0x01 0x02
**       0x27  ->  0x01 0x28
**
** If that were all the encoder did, it would work, but in certain cases
** it could double the size of the encoded string.  For example, to
** encode a string of 100 0x27 characters would require 100 instances of
** the 0x01 0x03 escape sequence resulting in a 200-character output.
** We would prefer to keep the size of the encoded string smaller than
** this.
**
** To minimize the encoding size, we first add a fixed offset value to each 
** byte in the sequence.  The addition is modulo 256.  (That is to say, if
** the sum of the original character value and the offset exceeds 256, then
** the higher order bits are truncated.)  The offset is chosen to minimize
** the number of characters in the string that need to be escaped.  For
** example, in the case above where the string was composed of 100 0x27
** characters, the offset might be 0x01.  Each of the 0x27 characters would
** then be converted into an 0x28 character which would not need to be
** escaped at all and so the 100 character input string would be converted
** into just 100 characters of output.  Actually 101 characters of output - 
** we have to record the offset used as the first byte in the sequence so
** that the string can be decoded.  Since the offset value is stored as
** part of the output string and the output string is not allowed to contain
** characters 0x00 or 0x27, the offset cannot be 0x00 or 0x27.
**
** Here, then, are the encoding steps:
**
**     (1)   Choose an offset value and make it the first character of
**           output.
**
**     (2)   Copy each input character into the output buffer, one by
**           one, adding the offset value as you copy.
**
**     (3)   If the value of an input character plus offset is 0x00, replace
**           that one character by the two-character sequence 0x01 0x01.
**           If the sum is 0x01, replace it with 0x01 0x02.  If the sum
**           is 0x27, replace it with 0x01 0x03.
**
**     (4)   Put a 0x00 terminator at the end of the output.
**
** Decoding is obvious:
**
**     (5)   Copy encoded characters except the first into the decode 
**           buffer.  Set the first encoded character aside for use as
**           the offset in step 7 below.
**
**     (6)   Convert each 0x01 0x01 sequence into a single character 0x00.
**           Convert 0x01 0x02 into 0x01.  Convert 0x01 0x28 into 0x27.
**
**     (7)   Subtract the offset value that was the first character of
**           the encoded buffer from all characters in the output buffer.
**
** The only tricky part is step (1) - how to compute an offset value to
** minimize the size of the output buffer.  This is accomplished by testing
** all offset values and picking the one that results in the fewest number
** of escapes.  To do that, we first scan the entire input and count the
** number of occurances of each character value in the input.  Suppose
** the number of 0x00 characters is N(0), the number of occurances of 0x01
** is N(1), and so forth up to the number of occurances of 0xff is N(255).
** An offset of 0 is not allowed so we don't have to test it.  The number
** of escapes required for an offset of 1 is N(1)+N(2)+N(40).  The number
** of escapes required for an offset of 2 is N(2)+N(3)+N(41).  And so forth.
** In this way we find the offset that gives the minimum number of escapes,
** and thus minimizes the length of the output string.
*/

/*
** Encode a binary buffer "in" of size n bytes so that it contains
** no instances of characters '\'' or '\000'.  The output is 
** null-terminated and can be used as a string value in an INSERT
** or UPDATE statement.  Use sqlite_decode_binary() to convert the
** string back into its original binary.
**
** The result is written into a preallocated output buffer "out".
** "out" must be able to hold at least 2 +(257*n)/254 bytes.
** In other words, the output will be expanded by as much as 3
** bytes for every 254 bytes of input plus 2 bytes of fixed overhead.
** (This is approximately 2 + 1.0118*n or about a 1.2% size increase.)
**
** The return value is the number of characters in the encoded
** string, excluding the "\000" terminator.
**
** If out==NULL then no output is generated but the routine still returns
** the number of characters that would have been generated if out had
** not been NULL.
*/
int sqlite_encode_binary(const unsigned char *in, int n, unsigned char *out){
  int i, j, e, m;
  unsigned char x;
  int cnt[256];
  if( n<=0 ){
    if( out ){
      out[0] = 'x';
      out[1] = 0;
    }
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
  if( out==0 ){
    return n+m+1;
  }
  out[0] = e;
  j = 1;
  for(i=0; i<n; i++){
    x = in[i] - e;
    if( x==0 || x==1 || x=='\''){
      out[j++] = 1;
      x++;
    }
    out[j++] = x;
  }
  out[j] = 0;
  assert( j==n+m+1 );
  return j;
}

/*
** Decode the string "in" into binary data and write it into "out".
** This routine reverses the encoding created by sqlite_encode_binary().
** The output will always be a few bytes less than the input.  The number
** of bytes of output is returned.  If the input is not a well-formed
** encoding, -1 is returned.
**
** The "in" and "out" parameters may point to the same buffer in order
** to decode a string in place.
*/
int sqlite_decode_binary(const unsigned char *in, unsigned char *out){
  int i, e;
  unsigned char c;
  e = *(in++);
  i = 0;
  while( (c = *(in++))!=0 ){
    if( c==1 ){
      c = *(in++) - 1;
    }
    out[i++] = c + e;
  }
  return i;
}

#ifdef ENCODER_TEST
#include <stdio.h>
/*
** The subroutines above are not tested by the usual test suite.  To test
** these routines, compile just this one file with a -DENCODER_TEST=1 option
** and run the result.
*/
int main(int argc, char **argv){
  int i, j, n, m, nOut, nByteIn, nByteOut;
  unsigned char in[30000];
  unsigned char out[33000];

  nByteIn = nByteOut = 0;
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
    nByteIn += n;
    nOut = sqlite_encode_binary(in, n, out);
    nByteOut += nOut;
    if( nOut!=strlen(out) ){
      printf(" ERROR return value is %d instead of %d\n", nOut, strlen(out));
      exit(1);
    }
    if( nOut!=sqlite_encode_binary(in, n, 0) ){
      printf(" ERROR actual output size disagrees with predicted size\n");
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
  fprintf(stderr,"Finished.  Total encoding: %d->%d bytes\n",
          nByteIn, nByteOut);
  fprintf(stderr,"Avg size increase: %.3f%%\n",
    (nByteOut-nByteIn)*100.0/(double)nByteIn);
}
#endif /* ENCODER_TEST */
