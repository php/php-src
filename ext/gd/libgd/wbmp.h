/* WBMP
** ----
** WBMP Level 0: B/W, Uncompressed
** This implements the WBMP format as specified in WAPSpec 1.1 and 1.2.
** It does not support ExtHeaders as defined in the spec. The spec states
** that a WAP client does not need to implement ExtHeaders.
**
** (c) 2000 Johan Van den Brande <johan@vandenbrande.com>
**
** Header file
*/        
#ifndef __WBMP_H
#define __WBMP_H	1


/* WBMP struct
** -----------
** A Wireless bitmap structure
**
*/
 
typedef struct Wbmp_
{
    int type;           /* type of the wbmp */
    int width;          /* width of the image */
    int height;         /* height of the image */
    int *bitmap;        /* pointer to data: 0 = WHITE , 1 = BLACK */
} Wbmp;
 
#define WBMP_WHITE  1
#define WBMP_BLACK  0
 

/* Proto's
** -------
**
*/
void		putmbi( int i, void (*putout)(int c, void *out), void *out); 
int 	getmbi ( int (*getin)(void *in), void *in );
int     skipheader( int (*getin)(void *in), void *in );
Wbmp   *createwbmp( int width, int height, int color );
int     readwbmp( int (*getin)(void *in), void *in, Wbmp **wbmp );
int		writewbmp( Wbmp *wbmp, void (*putout)( int c, void *out), void *out);
void    freewbmp( Wbmp *wbmp );
void    printwbmp( Wbmp *wbmp );  

#endif
