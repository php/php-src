
/* WBMP
   ** ----
   ** WBMP Level 0: B/W, Uncompressed
   ** This implements the WBMP format as specified in WAPSpec 1.1 and 1.2.
   ** It does not support ExtHeaders as defined in the spec. The spec states
   ** that a WAP client does not need to implement ExtHeaders.
   **
   ** (c) 2000 Johan Van den Brande <johan@vandenbrande.com>
 */


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "wbmp.h"
#include "gd.h"
#include "gdhelpers.h"

#ifdef NOTDEF
#define __TEST			/* Compile with main function */
#define __DEBUG			/* Extra verbose when with __TEST */
#define __WRITE			/* readwbmp and writewbmp(stdout) */
#define __VIEW			/* view the wbmp on stdout */
#endif

/* getmbi
   ** ------
   ** Get a multibyte integer from a generic getin function 
   ** 'getin' can be getc, with in = NULL
   ** you can find getin as a function just above the main function
   ** This way you gain a lot of flexibilty about how this package
   ** reads a wbmp file.
 */
int
getmbi (int (*getin) (void *in), void *in)
{
  int i, mbi = 0;

  do
    {
      i = getin (in);
      if (i < 0)
	return (-1);
      mbi = (mbi << 7) | (i & 0x7f);
    }
  while (i & 0x80);

  return (mbi);
}


/* putmbi
   ** ------
   ** Put a multibyte intgerer in some kind of output stream
   ** I work here with a function pointer, to make it as generic
   ** as possible. Look at this function as an iterator on the
   ** mbi integers it spits out.
   **
 */
void
putmbi (int i, void (*putout) (int c, void *out), void *out)
{
  int cnt, l, accu;

  /* Get number of septets */
  cnt = 0;
  accu = 0;
  while (accu != i)
    accu += i & 0x7f << 7 * cnt++;

  /* Produce the multibyte output */
  for (l = cnt - 1; l > 0; l--)
    putout (0x80 | (i & 0x7f << 7 * l) >> 7 * l, out);

  putout (i & 0x7f, out);

}



/* skipheader
   ** ----------
   ** Skips the ExtHeader. Not needed for the moment
   **
 */
int
skipheader (int (*getin) (void *in), void *in)
{
  int i;

  do
    {
      i = getin (in);
      if (i < 0)
	return (-1);
    }
  while (i & 0x80);

  return (0);
}

/* create wbmp
   ** -----------
   ** create an empty wbmp
   **
 */
Wbmp *
createwbmp (int width, int height, int color)
{
  int i;

  Wbmp *wbmp;
  if ((wbmp = (Wbmp *) gdMalloc (sizeof (Wbmp))) == NULL)
    return (NULL);

  if ((wbmp->bitmap = (int *) gdMalloc (sizeof (int) * width * height)) == NULL)
    {
      gdFree (wbmp);
      return (NULL);
    }

  wbmp->width = width;
  wbmp->height = height;

  for (i = 0; i < width * height; wbmp->bitmap[i++] = color);

  return (wbmp);
}



/* readwbmp
   ** -------
   ** Actually reads the WBMP format from an open file descriptor
   ** It goes along by returning a pointer to a WBMP struct.
   **
 */
int
readwbmp (int (*getin) (void *in), void *in, Wbmp ** return_wbmp)
{
  int row, col, byte, pel, pos;
  Wbmp *wbmp;

  if ((wbmp = (Wbmp *) gdMalloc (sizeof (Wbmp))) == NULL)
    return (-1);

  wbmp->type = getin (in);
  if (wbmp->type != 0)
    {
      gdFree (wbmp);
      return (-1);
    }

  if (skipheader (getin, in))
    return (-1);


  wbmp->width = getmbi (getin, in);
  if (wbmp->width == -1)
    {
      gdFree (wbmp);
      return (-1);
    }

  wbmp->height = getmbi (getin, in);
  if (wbmp->height == -1)
    {
      gdFree (wbmp);
      return (-1);
    }

#ifdef __DEBUG
  printf ("W: %d, H: %d\n", wbmp->width, wbmp->height);
#endif

  if ((wbmp->bitmap = (int *) gdMalloc (sizeof (int) * wbmp->width * wbmp->height)) == NULL)
    {
      gdFree (wbmp);
      return (-1);
    }

#ifdef __DEBUG
  printf ("DATA CONSTRUCTED\n");
#endif

  pos = 0;
  for (row = 0; row < wbmp->height; row++)
    {
      for (col = 0; col < wbmp->width;)
	{
	  byte = getin (in);

	  for (pel = 7; pel >= 0; pel--)
	    {
	      if (col++ < wbmp->width)
		{
		  if (byte & 1 << pel)
		    {
		      wbmp->bitmap[pos] = WBMP_WHITE;
		    }
		  else
		    {
		      wbmp->bitmap[pos] = WBMP_BLACK;
		    }
		  pos++;
		}
	    }
	}
    }

  *return_wbmp = wbmp;

  return (0);
}


/* writewbmp
   ** ---------
   ** Write a wbmp to a file descriptor
   **
   ** Why not just giving a filedescriptor to this function?
   ** Well, the incentive to write this function was the complete
   ** integration in gd library from www.boutell.com. They use
   ** their own io functions, so the passing of a function seemed to be 
   ** a logic(?) decision ...
   **
 */
int
writewbmp (Wbmp * wbmp, void (*putout) (int c, void *out), void *out)
{
  int row, col;
  int bitpos, octet;

  /* Generate the header */
  putout (0, out);		/* WBMP Type 0: B/W, Uncompressed bitmap */
  putout (0, out);		/* FixHeaderField */



  /* Size of the image */
  putmbi (wbmp->width, putout, out);	/* width */
  putmbi (wbmp->height, putout, out);	/* height */


  /* Image data */
  for (row = 0; row < wbmp->height; row++)
    {
      bitpos = 8;
      octet = 0;
      for (col = 0; col < wbmp->width; col++)
	{
	  octet |= ((wbmp->bitmap[row * wbmp->width + col] == 1) ? WBMP_WHITE : WBMP_BLACK) << --bitpos;
	  if (bitpos == 0)
	    {
	      bitpos = 8;
	      putout (octet, out);
	      octet = 0;
	    }
	}
      if (bitpos != 8)
	putout (octet, out);

    }
  return (0);

}


/* freewbmp
   ** --------
   ** gdFrees up memory occupied by a WBMP structure
   **
 */
void
freewbmp (Wbmp * wbmp)
{
  gdFree (wbmp->bitmap);
  gdFree (wbmp);
}


/* printwbmp
   ** ---------
   ** print a WBMP to stdout for visualisation
   **
 */
void
printwbmp (Wbmp * wbmp)
{
  int row, col;
  for (row = 0; row < wbmp->height; row++)
    {
      for (col = 0; col < wbmp->width; col++)
	{
	  if (wbmp->bitmap[wbmp->width * row + col] == WBMP_BLACK)
	    {
	      putchar ('#');
	    }
	  else
	    {
	      putchar (' ');
	    }
	}
      putchar ('\n');
    }
}

#ifdef __TEST

/* putout to file descriptor
   ** -------------------------
 */
int
putout (int c, void *out)
{
  return (putc (c, (FILE *) out));
}

/* getin from file descriptor 
   ** --------------------------
 */
int
getin (void *in)
{
  return (getc ((FILE *) in));
}


/* Main function
   ** -------------
   **
 */
int
main (int argc, char *argv[])
{
  FILE *wbmp_file;
  Wbmp *wbmp;

  wbmp_file = fopen (argv[1], "rb");
  if (wbmp_file)
    {
      readwbmp (&getin, wbmp_file, &wbmp);

#ifdef __VIEW

#ifdef __DEBUG
      printf ("\nVIEWING IMAGE\n");
#endif

      printwbmp (wbmp);
#endif

#ifdef __WRITE

#ifdef __DEBUG
      printf ("\nDUMPING WBMP to STDOUT\n");
#endif

      writewbmp (wbmp, &putout, stdout);
#endif

      freewbmp (wbmp);
      fclose (wbmp_file);
    }
}
#endif
