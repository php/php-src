/*************************************************
*     libucp - Unicode Property Table handler    *
*************************************************/

/* Internal header file defining the layout of compact nodes in the tree. */

typedef struct cnode {
  unsigned short int f0;
  unsigned short int f1;
  unsigned short int f2;
} cnode;

/* Things for the f0 field */

#define f0_leftexists   0x8000    /* Left child exists */
#define f0_typemask     0x3f00    /* Type bits */
#define f0_typeshift         8    /* Type shift */
#define f0_chhmask      0x00ff    /* Character high bits */

/* Things for the f2 field */

#define f2_rightmask    0xf000    /* Mask for right offset bits */
#define f2_rightshift       12    /* Shift for right offset */
#define f2_casemask     0x0fff    /* Mask for case offset */

/* The tree consists of a vector of structures of type cnode, with the root
node as the first element. The three short ints (16-bits) are used as follows:

(f0) (1) The 0x8000 bit of f0 is set if a left child exists. The child's node
         is the next node in the vector.
     (2) The 0x4000 bits of f0 is spare.
     (3) The 0x3f00 bits of f0 contain the character type; this is a number
         defined by the enumeration in ucp.h (e.g. ucp_Lu).
     (4) The bottom 8 bits of f0 contain the most significant byte of the
         character's 24-bit codepoint.

(f1) (1) The f1 field contains the two least significant bytes of the
         codepoint.

(f2) (1) The 0xf000 bits of f2 contain zero if there is no right child of this
         node. Otherwise, they contain one plus the exponent of the power of
         two of the offset to the right node (e.g. a value of 3 means 8). The
         units of the offset are node items.

     (2) The 0x0fff bits of f2 contain the signed offset from this character to
         its alternate cased value. They are zero if there is no such
         character.


-----------------------------------------------------------------------------
||.|.| type (6) | ms char (8) ||  ls char (16)  ||....|  case offset (12)  ||
-----------------------------------------------------------------------------
  | |                                              |
  | |-> spare                                      |
  |                                        exponent of right
  |-> left child exists                       child offset


The upper/lower casing information is set only for characters that come in
pairs. There are (at present) four non-one-to-one mappings in the Unicode data.
These are ignored. They are:

  1FBE Greek Prosgegrammeni (lower, with upper -> capital iota)
  2126 Ohm
  212A Kelvin
  212B Angstrom

Certainly for the last three, having an alternate case would seem to be a
mistake. I don't know any Greek, so cannot comment on the first one.


When searching the tree, proceed as follows:

(1) Start at the first node.

(2) Extract the character value from f1 and the bottom 8 bits of f0;

(3) Compare with the character being sought. If equal, we are done.

(4) If the test character is smaller, inspect the f0_leftexists flag. If it is
    not set, the character is not in the tree. If it is set, move to the next
    node, and go to (2).

(5) If the test character is bigger, extract the f2_rightmask bits from f2, and
    shift them right by f2_rightshift. If the result is zero, the character is
    not in the tree. Otherwise, calculate the number of nodes to skip by
    shifting the value 1 left by this number minus one. Go to (2).
*/


/* End of internal.h */
