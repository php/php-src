/*
 * UFC-crypt: ultra fast crypt(3) implementation
 *
 * Copyright (C) 1991, Michael Glad, email: glad@daimi.aau.dk
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * @(#)crypt.c	2.2 10/04/91
 *
 * Semiportable C version
 *
 */

#include <string.h>

#define bzero(addr, cnt)     memset(addr, 0, cnt)
#define bcopy(from, to, len) memcpy(to, from, len)

/* Permutation done once on the 56 bit 
   key derived from the original 8 byte ASCII key.
*/
static unsigned long pc1[56] =
  { 57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
    10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
    14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4
  };

/* How much to rotate each 28 bit half of the pc1 permutated
   56 bit key before using pc2 to give the i' key
*/
static unsigned long totrot[16] =
  { 1, 2, 4, 6, 8, 10, 12, 14, 15, 17, 19, 21, 23, 25, 27, 28 };

/* Permutation giving the key of the i' DES round */
static unsigned long pc2[48] =
  { 14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
  };

/* Reference copy of the expansion table which selects
   bits from the 32 bit intermediate result.
*/
static unsigned long eref[48] =
  { 32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
     8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
  };
static unsigned long disturbed_e[48];
static unsigned long e_inverse[64];

/* Permutation done on the result of sbox lookups */
static unsigned long perm32[32] = 
  { 16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10,
     2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25
  };

/* The sboxes */
static unsigned long sbox[8][4][16]=
      { { { 14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7 },
          {  0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8 },
          {  4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0 },
          { 15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13 }
        },

        { { 15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10 },
          {  3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5 },
          {  0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15 },
          { 13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9 }
        },

        { { 10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8 },
          { 13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1 },
          { 13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7 },
          {  1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12 }
        },

        { {  7,  13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15 },
          { 13,  8,  11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9 },
          { 10,  6,   9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4 },
          {  3, 15,   0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14 }
        },

        { {  2, 12,   4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9 },
          { 14, 11,   2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6 },
          {  4,  2,   1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14 },
          { 11,  8,  12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3 }
        },

        { { 12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11 },
          { 10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8 },
          {  9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6 },
          {  4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13 }
        },

        { {  4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1 },
          { 13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6 },
          {  1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2 },
          {  6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12 }
        },

        { { 13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7 },
          {  1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2 },
          {  7, 11, 4,   1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8 },
          {  2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11 }
        }
      };

#ifdef notdef

/* This is the initial permutation matrix -- we have no
   use for it, but it is needed if you will develop
   this module into a general DES package.
*/
static unsigned char inital_perm[64] = 
  { 58, 50, 42, 34, 26, 18, 10,  2, 60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14,  6, 64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15, 7
  };

#endif

/* Final permutation matrix -- not used directly */
static unsigned char final_perm[64] =
  { 40,  8, 48, 16, 56, 24, 64, 32, 39,  7, 47, 15, 55, 23, 63, 31,
    38,  6, 46, 14, 54, 22, 62, 30, 37,  5, 45, 13, 53, 21, 61, 29,
    36,  4, 44, 12, 52, 20, 60, 28, 35,  3, 43, 11, 51, 19, 59, 27,
    34,  2, 42, 10, 50, 18, 58, 26, 33,  1, 41,  9, 49, 17, 57, 25
  };

/* The 16 DES keys in BITMASK format */
unsigned long keytab[16][2];

#define ascii_to_bin(c) ((c)>='a'?(c-59):(c)>='A'?((c)-53):(c)-'.')
#define bin_to_ascii(c) ((c)>=38?((c)-38+'a'):(c)>=12?((c)-12+'A'):(c)+'.')

/* Macro to set a bit (0..23) */
#define BITMASK(i) ( (1<<(11-(i)%12+3)) << ((i)<12?16:0) )

/* sb arrays:

   Workhorses of the inner loop of the DES implementation.
   They do sbox lookup, shifting of this  value, 32 bit
   permutation and E permutation for the next round.

   Kept in 'BITMASK' format.

*/

unsigned long sb0[8192],sb1[8192],sb2[8192],sb3[8192];
static unsigned long *sb[4] = {sb0,sb1,sb2,sb3}; 

/* eperm32tab: do 32 bit permutation and E selection

   The first index is the byte number in the 32 bit value to be permuted
    -  second  -   is the value of this byte
    -  third   -   selects the two 32 bit values

    The table is used and generated internally in init_des to speed it up

*/
static unsigned long eperm32tab[4][256][2];

/* mk_keytab_table: fast way of generating keytab from ASCII key

   The first  index is the byte number in the 8 byte ASCII key
    -  second   -    -  -  current DES round i.e. the key number
    -  third    -   distinguishes between the two 24 bit halfs of
                    the selected key
    -  fourth   -   selects the 7 bits actually used of each byte

   The table is kept in the format generated by the BITMASK macro

*/
static unsigned long mk_keytab_table[8][16][2][128];


/* efp: undo an extra e selection and do final
        permutation giving the DES result.
  
        Invoked 6 bit a time on two 48 bit values
        giving two 32 bit longs.
*/
static unsigned long efp[16][64][2];


static unsigned char bytemask[8]  =
  { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };


static unsigned long longmask[32] = 
  { 0x80000000, 0x40000000, 0x20000000, 0x10000000,
    0x08000000, 0x04000000, 0x02000000, 0x01000000,
    0x00800000, 0x00400000, 0x00200000, 0x00100000,
    0x00080000, 0x00040000, 0x00020000, 0x00010000,
    0x00008000, 0x00004000, 0x00002000, 0x00001000,
    0x00000800, 0x00000400, 0x00000200, 0x00000100,
    0x00000080, 0x00000040, 0x00000020, 0x00000010,
    0x00000008, 0x00000004, 0x00000002, 0x00000001
  };

static unsigned long initialized = 0;

/* lookup a 6 bit value in sbox */

#define s_lookup(i,s) sbox[(i)][(((s)>>4) & 0x2)|((s) & 0x1)][((s)>>1) & 0xf];

/* Generate the mk_keytab_table once in a program execution */

void init_des()
  { unsigned long tbl_long,bit_within_long,comes_from_bit;
    unsigned long bit,sg,j;
    unsigned long bit_within_byte,key_byte,byte_value;
    unsigned long round,mask;

    bzero((char*)mk_keytab_table,sizeof mk_keytab_table);
    
    for(round=0; round<16; round++)
      for(bit=0; bit<48; bit++)
        { tbl_long        = bit / 24;
          bit_within_long = bit % 24;

          /* from which bit in the key halves does it origin? */
          comes_from_bit = pc2[bit] - 1;

          /* undo the rotation done before pc2 */
          if(comes_from_bit>=28)
            comes_from_bit =  28 + (comes_from_bit + totrot[round]) % 28;
          else
            comes_from_bit =       (comes_from_bit + totrot[round]) % 28;

          /* undo the initial key half forming permutation */
          comes_from_bit = pc1[comes_from_bit] - 1;

          /* Now 'comes_from_bit' is the correct number (0..55) 
             of the keybit from which the bit being traced
             in key 'round' comes from
          */
 
          key_byte        =  comes_from_bit  / 8;
          bit_within_byte = (comes_from_bit % 8)+1;

          mask = bytemask[bit_within_byte];

          for(byte_value=0; byte_value<128; byte_value++)
            if(byte_value & mask)
              mk_keytab_table[key_byte][round][tbl_long][byte_value] |= 
		BITMASK(bit_within_long);
        }

    /* Now generate the table used to do an combined
       32 bit permutation and e expansion

       We use it because we have to permute 16384 32 bit
       longs into 48 bit in order to initialize sb.

       Looping 48 rounds per permutation becomes 
       just too slow...

    */

    bzero((char*)eperm32tab,sizeof eperm32tab);
    for(bit=0; bit<48; bit++)
      { unsigned long mask1,comes_from;
	
        comes_from = perm32[eref[bit]-1]-1;
        mask1      = bytemask[comes_from % 8];
	
        for(j=256; j--;)
          if(j & mask1)
            eperm32tab[comes_from/8][j][bit/24] |= BITMASK(bit % 24);
      }
    
    /* Create the sb tables:

       For each 12 bit segment of an 48 bit intermediate
       result, the sb table precomputes the two 4 bit
       values of the sbox lookups done with the two 6
       bit halves, shifts them to their proper place,
       sends them through perm32 and finally E expands
       them so that they are ready for the next
       DES round.

       The value looked up is to be xored onto the
       two 48 bit right halves.
    */

    for(sg=0; sg<4; sg++)
      { unsigned long j1,j2;
        unsigned long s1,s2;
    
        for(j1=0; j1<64; j1++)
          { s1 = s_lookup(2*sg,j1);
            for(j2=0; j2<64; j2++)
              { unsigned long to_permute,inx;

                s2         = s_lookup(2*sg+1,j2);
                to_permute = ((s1<<4)  | s2) << (24-8*sg);
                inx        = ((j1<<6)  | j2) << 1;

                sb[sg][inx  ]  = eperm32tab[0][(to_permute >> 24) & 0xff][0];
                sb[sg][inx+1]  = eperm32tab[0][(to_permute >> 24) & 0xff][1];
  
                sb[sg][inx  ] |= eperm32tab[1][(to_permute >> 16) & 0xff][0];
                sb[sg][inx+1] |= eperm32tab[1][(to_permute >> 16) & 0xff][1];
  
                sb[sg][inx  ] |= eperm32tab[2][(to_permute >>  8) & 0xff][0];
                sb[sg][inx+1] |= eperm32tab[2][(to_permute >>  8) & 0xff][1];
                
                sb[sg][inx  ] |= eperm32tab[3][(to_permute)       & 0xff][0];
                sb[sg][inx+1] |= eperm32tab[3][(to_permute)       & 0xff][1];
              }
          }
      }  
    initialized++;
  }

/* Process the elements of the sb table permuting the
   bits swapped in the expansion by the current salt.
*/

void shuffle_sb(k, saltbits)
  unsigned long *k, saltbits;
  { int j, x;
    for(j=4096; j--;) {
      x = (k[0] ^ k[1]) & saltbits;
      *k++ ^= x;
      *k++ ^= x;
    }
  }

/* Setup the unit for a new salt
   Hopefully we'll not see a new salt in each crypt call.
*/

static unsigned char current_salt[3]="&&"; /* invalid value */
static unsigned long oldsaltbits = 0;

void setup_salt(s)
  char *s;
  { unsigned long i,j,saltbits;

    if(!initialized)
      init_des();

    if(s[0]==current_salt[0] && s[1]==current_salt[1])
      return;
    current_salt[0]=s[0]; current_salt[1]=s[1];

    /* This is the only crypt change to DES:
       entries are swapped in the expansion table
       according to the bits set in the salt.
    */

    saltbits=0;
    bcopy((char*)eref,(char*)disturbed_e,sizeof eref);
    for(i=0; i<2; i++)
      { long c=ascii_to_bin(s[i]);
	if(c<0 || c>63)
	  c=0;
        for(j=0; j<6; j++)
          if((c>>j) & 0x1)
            { disturbed_e[6*i+j   ]=eref[6*i+j+24];
              disturbed_e[6*i+j+24]=eref[6*i+j   ];
	      saltbits |= BITMASK(6*i+j);
            }
      }

    /* Permute the sb table values
       to reflect the changed e
       selection table
    */

    shuffle_sb(sb0, oldsaltbits ^ saltbits); 
    shuffle_sb(sb1, oldsaltbits ^ saltbits);
    shuffle_sb(sb2, oldsaltbits ^ saltbits);
    shuffle_sb(sb3, oldsaltbits ^ saltbits);

    oldsaltbits = saltbits;

    /* Create an inverse matrix for disturbed_e telling
       where to plug out bits if undoing disturbed_e
    */

    for(i=48; i--;)
      { e_inverse[disturbed_e[i]-1   ] = i;
	e_inverse[disturbed_e[i]-1+32] = i+48;
      }

    /* create efp: the matrix used to
       undo the E expansion and effect final permutation
    */

    bzero((char*)efp,sizeof efp);
    for(i=0; i<64; i++)
      { unsigned long o_bit,o_long;
        unsigned long word_value,mask1,mask2,comes_from_f_bit,comes_from_e_bit;
        unsigned long comes_from_word,bit_within_word;

	/* See where bit i belongs in the two 32 bit long's */
	o_long = i / 32; /* 0..1  */
	o_bit  = i % 32; /* 0..31 */

	/* And find a bit in the e permutated value setting this bit.

	   Note: the e selection may have selected the same bit several
	   times. By the initialization of e_inverse, we only look
	   for one specific instance.
	*/
        comes_from_f_bit = final_perm[i]-1;             /* 0..63 */
	comes_from_e_bit = e_inverse[comes_from_f_bit]; /* 0..95 */
        comes_from_word  = comes_from_e_bit / 6;        /* 0..15 */
        bit_within_word  = comes_from_e_bit % 6;        /* 0..5  */

	mask1 = longmask[bit_within_word+26];
	mask2 = longmask[o_bit];

        for(word_value=64; word_value--;)
          if(word_value & mask1)
            efp[comes_from_word][word_value][o_long] |= mask2;

      }

  }

/* Generate the key table before running the 25 DES rounds */

void mk_keytab(key)
  char *key;
  { unsigned long i,j;
    unsigned long *k,*mkt;
    char t;

    bzero((char*)keytab, sizeof keytab);
    mkt = &mk_keytab_table[0][0][0][0];

    for(i=0; (t=(*key++) & 0x7f) && i<8; i++)
      for(j=0,k = &keytab[0][0]; j<16; j++)
        { *k++ |= mkt[t]; mkt += 128;
          *k++ |= mkt[t]; mkt += 128;
        }
    for(; i<8; i++)
      for(j=0,k = &keytab[0][0]; j<16; j++)
        { *k++ |= mkt[0]; mkt += 128;
          *k++ |= mkt[0]; mkt += 128;
        }
  }

/* Do final permutations and convert to ASCII */

char *output_conversion(l1,l2,r1,r2,salt)
  unsigned long l1,l2,r1,r2;
  char *salt;
  { static char outbuf[14];
    unsigned long i;
    unsigned long s,v1,v2;

    /* Unfortunately we've done an extra E
       expansion -- undo it at the same time.
    */

    v1=v2=0; l1 >>= 3; l2 >>= 3; r1 >>= 3; r2 >>= 3;

    v1 |= efp[ 3][ l1       & 0x3f][0]; v2 |= efp[ 3][ l1 & 0x3f][1];
    v1 |= efp[ 2][(l1>>=6)  & 0x3f][0]; v2 |= efp[ 2][ l1 & 0x3f][1];
    v1 |= efp[ 1][(l1>>=10) & 0x3f][0]; v2 |= efp[ 1][ l1 & 0x3f][1];
    v1 |= efp[ 0][(l1>>=6)  & 0x3f][0]; v2 |= efp[ 0][ l1 & 0x3f][1];

    v1 |= efp[ 7][ l2       & 0x3f][0]; v2 |= efp[ 7][ l2 & 0x3f][1];
    v1 |= efp[ 6][(l2>>=6)  & 0x3f][0]; v2 |= efp[ 6][ l2 & 0x3f][1];
    v1 |= efp[ 5][(l2>>=10) & 0x3f][0]; v2 |= efp[ 5][ l2 & 0x3f][1];
    v1 |= efp[ 4][(l2>>=6)  & 0x3f][0]; v2 |= efp[ 4][ l2 & 0x3f][1];

    v1 |= efp[11][ r1       & 0x3f][0]; v2 |= efp[11][ r1 & 0x3f][1];
    v1 |= efp[10][(r1>>=6)  & 0x3f][0]; v2 |= efp[10][ r1 & 0x3f][1];
    v1 |= efp[ 9][(r1>>=10) & 0x3f][0]; v2 |= efp[ 9][ r1 & 0x3f][1];
    v1 |= efp[ 8][(r1>>=6)  & 0x3f][0]; v2 |= efp[ 8][ r1 & 0x3f][1];

    v1 |= efp[15][ r2       & 0x3f][0]; v2 |= efp[15][ r2 & 0x3f][1];
    v1 |= efp[14][(r2>>=6)  & 0x3f][0]; v2 |= efp[14][ r2 & 0x3f][1];
    v1 |= efp[13][(r2>>=10) & 0x3f][0]; v2 |= efp[13][ r2 & 0x3f][1];
    v1 |= efp[12][(r2>>=6)  & 0x3f][0]; v2 |= efp[12][ r2 & 0x3f][1];
    
    outbuf[0] = salt[0];
    outbuf[1] = salt[1] ? salt[1] : salt[0];

    for(i=0; i<5; i++)
      outbuf[i+2] = bin_to_ascii((v1>>(26-6*i)) & 0x3f);

    s  = (v2 & 0xf) << 2;             /* Save the rightmost 4 bit a moment */
    v2 = (v2>>2) | ((v1 & 0x3)<<30);  /* Shift two bits of v1 onto v2      */

    for(i=5; i<10; i++)
      outbuf[i+2] = bin_to_ascii((v2>>(56-6*i)) & 0x3f);

    outbuf[12] = bin_to_ascii(s);
    outbuf[13] = 0;

    return outbuf;
  }

#define SBA(sb, v) (*(unsigned long*)((char*)(sb)+(v)))

#define F(I, O1, O2, SBX, SBY)                                        \
    s = *k++ ^ I;                                                     \
    O1 ^= SBA(SBX, (s & 0xffff)); O2 ^= SBA(SBX, ((s & 0xffff) + 4)); \
    O1 ^= SBA(SBY, (s >>= 16));   O2 ^= SBA(SBY, ((s)          + 4));

#define G(I1, I2, O1, O2)                                             \
        F(I1, O1, O2, sb1, sb0) F(I2, O1, O2, sb3, sb2)

#define H G(r1, r2, l1, l2) ; G(l1, l2, r1, r2)

char *des_crypt(key, salt)
  char *key;
  char *salt;
  { unsigned long l1, l2, r1, r2, i, j, s, *k;

    setup_salt(salt);
    mk_keytab(key);

    l1=l2=r1=r2=0;

    for(j=0; j<25; j++) {
      k = &keytab[0][0];
      for(i=8; i--; ) {
	H;
      }
      s=l1; l1=r1; r1=s; s=l2; l2=r2; r2=s;
    }

    return output_conversion(l1, l2, r1, r2, salt);
  }

#include "php.h"
#include "md5crypt.h"

PHPAPI char *
crypt (const char *pw, const char *salt)
{
	if (strlen(salt)>MD5_MAGIC_LEN && strncmp(salt, MD5_MAGIC, MD5_MAGIC_LEN)==0) {
		return md5_crypt(pw, salt);
	} else {
		return des_crypt(pw, salt);
	}
}
