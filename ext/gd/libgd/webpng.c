/* Bring in the gd library functions */
#include "gd.h"

/* Bring in standard I/O and string manipulation functions */
#include <stdio.h>
#include <stdlib.h>		/* for atoi() */
#include <string.h>

#ifdef _WIN32
#include <process.h>
int
getpid ()
{
  return _getpid ();
}
#else
#include <unistd.h>		/* for getpid(), unlink() */
#endif
int
main (int argc, char **argv)
{
  FILE *in;
  FILE *out;
  char outFn[20];
  int useStdinStdout = 0;

  /* Declare our image pointer */
  gdImagePtr im = 0;
  int i;
  /* We'll clear 'no' once we know the user has made a
     reasonable request. */
  int no = 1;
  /* We'll set 'write' once we know the user's request
     requires that the image be written back to disk. */
  int write = 0;
  /* C programs always get at least one argument; we want at
     least one more (the image), more in practice. */
  if (argc < 2 || !strcmp (argv[1], "--help"))
    {
      no = 1;
      goto usage;
    }

  /* The last argument should be the image. Open the file. */
  if (strcmp ("-", argv[argc - 1]) == 0)
    {				/* - is synonymous with STDIN */
      useStdinStdout = 1;
      in = stdin;
    }
  else
    {
      in = fopen (argv[argc - 1], "rb");
    }
  if (!in)
    {
      fprintf (stderr,
	       "Error: can't open file %s.\n", argv[argc - 1]);
      exit (1);
    }
  /* Now load the image. */
  im = gdImageCreateFromPng (in);
  fclose (in);
  /* If the load failed, it must not be a PNG file. */
  if (!im)
    {
      fprintf (stderr,
	       "Error: %s is not a valid PNG file.\n", argv[argc - 1]);
      exit (1);
    }
  /* Consider each argument in turn. */
  for (i = 1; (i < (argc - 1)); i++)
    {
      /* -i turns on and off interlacing. */
      if (!strcmp (argv[i], "--help"))
	{
	  /* Every program should use this for help! :) */
	  no = 1;
	  goto usage;
	}
      else if (!strcmp (argv[i], "-i"))
	{
	  if (i == (argc - 2))
	    {
	      fprintf (stderr,
		       "Error: -i specified without y or n.\n");
	      no = 1;
	      goto usage;
	    }
	  if (!strcmp (argv[i + 1], "y"))
	    {
	      /* Set interlace. */
	      gdImageInterlace (im, 1);
	    }
	  else if (!strcmp (argv[i + 1], "n"))
	    {
	      /* Clear interlace. */
	      gdImageInterlace (im, 0);
	    }
	  else
	    {
	      fprintf (stderr,
		       "Error: -i specified without y or n.\n");
	      no = 1;
	      goto usage;
	    }
	  i++;
	  no = 0;
	  write = 1;
	}
      else if (!strcmp (argv[i], "-t"))
	{
	  /* Set transparent index (or none). */
	  int index;
	  if (i == (argc - 2))
	    {
	      fprintf (stderr,
		       "Error: -t specified without a color table index.\n");
	      no = 1;
	      goto usage;
	    }
	  if (!strcmp (argv[i + 1], "none"))
	    {
	      /* -1 means not transparent. */
	      gdImageColorTransparent (im, -1);
	    }
	  else
	    {
	      /* OK, get an integer and set the index. */
	      index = atoi (argv[i + 1]);
	      gdImageColorTransparent (im, index);
	    }
	  i++;
	  write = 1;
	  no = 0;
	}
      else if (!strcmp (argv[i], "-l"))
	{
	  /* List the colors in the color table. */
	  int j;
	  if (!im->trueColor)
	    {
	      /* Tabs used below. */
	      printf ("Index	Red	Green	Blue Alpha\n");
	      for (j = 0; (j < gdImageColorsTotal (im)); j++)
		{
		  /* Use access macros to learn colors. */
		  printf ("%d	%d	%d	%d	%d\n",
			  j,
			  gdImageRed (im, j),
			  gdImageGreen (im, j),
			  gdImageBlue (im, j),
			  gdImageAlpha (im, j));
		}
	    }
	  else
	    {
	      printf ("Truecolor image, no palette entries to list.\n");
	    }
	  no = 0;
	}
      else if (!strcmp (argv[i], "-d"))
	{
	  /* Output dimensions, etc. */
	  int t;
	  printf ("Width: %d Height: %d Colors: %d\n",
		  gdImageSX (im), gdImageSY (im),
		  gdImageColorsTotal (im));
	  t = gdImageGetTransparent (im);
	  if (t != (-1))
	    {
	      printf ("First 100% transparent index: %d\n", t);
	    }
	  else
	    {
	      /* -1 means the image is not transparent. */
	      printf ("First 100% transparent index: none\n");
	    }
	  if (gdImageGetInterlaced (im))
	    {
	      printf ("Interlaced: yes\n");
	    }
	  else
	    {
	      printf ("Interlaced: no\n");
	    }
	  no = 0;
	}
      else
	{
	  fprintf (stderr, "Unknown argument: %s\n", argv[i]);
	  break;
	}
    }
usage:
  if (no)
    {
      /* If the command failed, output an explanation. */
      fprintf (stderr,
	  "Usage: webpng [-i y|n ] [-l] [-t index|none ] [-d] pngname.png\n"

	       "  -i [y|n]   Turns on/off interlace\n"
	       "  -l         Prints the table of color indexes\n"
	       "  -t [index] Set the transparent color to the specified index (0-255 or \"none\")\n"
	       "  -d         Reports the dimensions and other characteristics of the image.\n"
	       "\n"
	       "If you specify '-' as the input file, stdin/stdout will be used input/output.\n"
	);
    }
  if (write)
    {
      if (useStdinStdout)
	{
	  out = stdout;
	}
      else
	{
	  /* Open a temporary file. */

	  /* "temp.tmp" is not good temporary filename. */
	  sprintf (outFn, "webpng.tmp%d", getpid ());
	  out = fopen (outFn, "wb");

	  if (!out)
	    {
	      fprintf (stderr,
		       "Unable to write to %s -- exiting\n", outFn);
	      exit (1);
	    }
	}

      /* Write the new PNG. */
      gdImagePng (im, out);

      if (!useStdinStdout)
	{
	  fclose (out);
	  /* Erase the old PNG. */
	  unlink (argv[argc - 1]);
	  /* Rename the new to the old. */
	  if (rename (outFn, argv[argc - 1]) != 0)
	    {
	      perror ("rename");
	      exit (1);
	    }
	}
    }
  /* Delete the image from memory. */
  if (im)
    {
      gdImageDestroy (im);
    }
  /* All's well that ends well. */
  return 0;
}
