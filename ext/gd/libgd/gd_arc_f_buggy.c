/* This is potentially great stuff, but fails against the test
   program at the end. This would probably be much more 
   efficent than the implementation currently in gd.c if the 
   errors in the output were corrected. TBB */

#if 0

#include "gd.h"
#include <math.h>

/* Courtesy of F J Franklin. */

static gdPoint gdArcClosest (int width, int height, int angle);

void
gdImageFilledEllipse (gdImagePtr im, int cx, int cy, int width, int height, int color)
{
  gdImageFilledArc (im, cx, cy, width, height, 0, 360, color, gdChord);
}

void
gdImageFilledArc (gdImagePtr im, int cx, int cy, int width, int height, int s, int e, int color, int style)
{
  gdPoint pt[7];
  gdPoint axis_pt[4];

  int angle;

  int have_s = 0;
  int have_e = 0;

  int flip_x = 0;
  int flip_y = 0;

  int conquer = 0;

  int i;

  int a;
  int b;

  int x;
  int y;

  long s_sin = 0;
  long s_cos = 0;
  long e_sin = 0;
  long e_cos = 0;

  long w;			/* a * 2 */
  long h;			/* b * 2 */

  long x2;			/* x * 2 */
  long y2;			/* y * 2 */
  long lx2;			/* x * 2 (line) */
  long ly2;			/* y * 2 (line) */

  long ws;			/* (a * 2)^2 */
  long hs;			/* (b * 2)^2 */

  long whs;			/* (a * 2)^2 * (b * 2)^2 */

  long g;			/* decision variable */
  long lg;			/* decision variable (line) */

  width = (width & 1) ? (width + 1) : (width);
  height = (height & 1) ? (height + 1) : (height);

  a = width / 2;
  b = height / 2;

  axis_pt[0].x = a;
  axis_pt[0].y = 0;
  axis_pt[1].x = 0;
  axis_pt[1].y = b;
  axis_pt[2].x = -a;
  axis_pt[2].y = 0;
  axis_pt[3].x = 0;
  axis_pt[3].y = -b;

  if (s == e)
    return;

  if ((e - s) >= 360)
    {
      s = 0;
      e = 0;
    }

  while (s < 0)
    s += 360;
  while (s >= 360)
    s -= 360;
  while (e < 0)
    e += 360;
  while (e >= 360)
    e -= 360;

  if (e <= s)
    e += 360;

  /* I'm assuming a chord-rule at the moment. Need to add origin to get a
   * pie-rule, but will need to set chord-rule before recursion...
   */

  for (i = 0; i < 4; i++)
    {
      if ((s < (i + 1) * 90) && (e > (i + 1) * 90))
	{
	  gdImageFilledArc (im, cx, cy, width, height, s, (i + 1) * 90, color, gdChord);
	  pt[0] = gdArcClosest (width, height, s);
	  pt[0].x += cx;
	  pt[0].y += cy;
	  pt[1].x = cx + axis_pt[(i + 1) & 3].x;
	  pt[1].y = cy + axis_pt[(i + 1) & 3].y;
	  if (e <= (i + 2) * 90)
	    {
	      gdImageFilledArc (im, cx, cy, width, height, (i + 1) * 90, e, color, gdChord);
	      pt[2] = gdArcClosest (width, height, e);
	      pt[2].x += cx;
	      pt[2].y += cy;
	      if (style == gdChord)
		{
		  gdImageFilledPolygon (im, pt, 3, color);
		  gdImagePolygon (im, pt, 3, color);
		}
	      else if (style == gdPie)
		{
		  pt[3].x = cx;
		  pt[3].y = cy;
		  gdImageFilledPolygon (im, pt, 4, color);
		  gdImagePolygon (im, pt, 4, color);
		}
	    }
	  else
	    {
	      gdImageFilledArc (im, cx, cy, width, height, (i + 1) * 90, (i + 2) * 90, color, gdChord);
	      pt[2].x = cx + axis_pt[(i + 2) & 3].x;
	      pt[2].y = cy + axis_pt[(i + 2) & 3].y;
	      if (e <= (i + 3) * 90)
		{
		  gdImageFilledArc (im, cx, cy, width, height, (i + 2) * 90, e, color, gdChord);
		  pt[3] = gdArcClosest (width, height, e);
		  pt[3].x += cx;
		  pt[3].y += cy;
		  if (style == gdChord)
		    {
		      gdImageFilledPolygon (im, pt, 4, color);
		      gdImagePolygon (im, pt, 4, color);
		    }
		  else if (style == gdPie)
		    {
		      pt[4].x = cx;
		      pt[4].y = cy;
		      gdImageFilledPolygon (im, pt, 5, color);
		      gdImagePolygon (im, pt, 5, color);
		    }
		}
	      else
		{
		  gdImageFilledArc (im, cx, cy, width, height, (i + 2) * 90, (i + 3) * 90, color, gdChord);
		  pt[3].x = cx + axis_pt[(i + 3) & 3].x;
		  pt[3].y = cy + axis_pt[(i + 3) & 3].y;
		  if (e <= (i + 4) * 90)
		    {
		      gdImageFilledArc (im, cx, cy, width, height, (i + 3) * 90, e, color, gdChord);
		      pt[4] = gdArcClosest (width, height, e);
		      pt[4].x += cx;
		      pt[4].y += cy;
		      if (style == gdChord)
			{
			  gdImageFilledPolygon (im, pt, 5, color);
			  gdImagePolygon (im, pt, 5, color);
			}
		      else if (style == gdPie)
			{
			  pt[5].x = cx;
			  pt[5].y = cy;
			  gdImageFilledPolygon (im, pt, 6, color);
			  gdImagePolygon (im, pt, 6, color);
			}
		    }
		  else
		    {
		      gdImageFilledArc (im, cx, cy, width, height, (i + 3) * 90, (i + 4) * 90, color, gdChord);
		      pt[4].x = cx + axis_pt[(i + 4) & 3].x;
		      pt[4].y = cy + axis_pt[(i + 4) & 3].y;

		      gdImageFilledArc (im, cx, cy, width, height, (i + 4) * 90, e, color, gdChord);
		      pt[5] = gdArcClosest (width, height, e);
		      pt[5].x += cx;
		      pt[5].y += cy;
		      if (style == gdChord)
			{
			  gdImageFilledPolygon (im, pt, 6, color);
			  gdImagePolygon (im, pt, 6, color);
			}
		      else if (style == gdPie)
			{
			  pt[6].x = cx;
			  pt[6].y = cy;
			  gdImageFilledPolygon (im, pt, 7, color);
			  gdImagePolygon (im, pt, 7, color);
			}
		    }
		}
	    }
	  return;
	}
    }

  /* At this point we have only arcs that lies within a quadrant -
   * map this to first quadrant...
   */

  if ((s >= 90) && (e <= 180))
    {
      angle = s;
      s = 180 - e;
      e = 180 - angle;
      flip_x = 1;
    }
  if ((s >= 180) && (e <= 270))
    {
      s = s - 180;
      e = e - 180;
      flip_x = 1;
      flip_y = 1;
    }
  if ((s >= 270) && (e <= 360))
    {
      angle = s;
      s = 360 - e;
      e = 360 - angle;
      flip_y = 1;
    }

  if (s == 0)
    {
      s_sin = 0;
      s_cos = (long) ((double) 32768);
    }
  else
    {
      s_sin = (long) ((double) 32768 * sin ((double) s * M_PI / (double) 180));
      s_cos = (long) ((double) 32768 * cos ((double) s * M_PI / (double) 180));
    }
  if (e == 0)
    {
      e_sin = (long) ((double) 32768);
      e_cos = 0;
    }
  else
    {
      e_sin = (long) ((double) 32768 * sin ((double) e * M_PI / (double) 180));
      e_cos = (long) ((double) 32768 * cos ((double) e * M_PI / (double) 180));
    }

  w = (long) width;
  h = (long) height;

  ws = w * w;
  hs = h * h;

  whs = 1;
  while ((ws > 32768) || (hs > 32768))
    {
      ws = (ws + 1) / 2;	/* Unfortunate limitations on integers makes */
      hs = (hs + 1) / 2;	/* drawing large  ellipses problematic...    */
      whs *= 2;
    }
  while ((ws * hs) > (0x04000000L / whs))
    {
      ws = (ws + 1) / 2;
      hs = (hs + 1) / 2;
      whs *= 2;
    }
  whs *= ws * hs;

  pt[0].x = w / 2;
  pt[0].y = 0;

  pt[2].x = 0;
  pt[2].y = h / 2;

  have_s = 0;
  have_e = 0;

  if (s == 0)
    have_s = 1;
  if (e == 90)
    have_e = 1;

  x2 = w;
  y2 = 0;			/* Starting point is exactly on ellipse */

  g = x2 - 1;
  g = g * g * hs + 4 * ws - whs;

  while ((x2 * hs) > (y2 * ws))	/* Keep |tangent| > 1 */
    {
      y2 += 2;
      g += ws * 4 * (y2 + 1);

      if (g > 0)		/* Need to drop */
	{
	  x2 -= 2;
	  g -= hs * 4 * x2;
	}

      if ((have_s == 0) && ((s_sin * x2) <= (y2 * s_cos)))
	{
	  pt[0].x = (int) (x2 / 2);
	  pt[0].y = (int) (y2 / 2);
	  have_s = 1;
	}

      if ((have_e == 0) && ((e_sin * x2) <= (y2 * e_cos)))
	{
	  pt[2].x = (int) (x2 / 2);
	  pt[2].y = (int) (y2 / 2);
	  have_e = 1;
	}
    }
  pt[1].x = (int) (x2 / 2);
  pt[1].y = (int) (y2 / 2);

  x2 = 0;
  y2 = h;			/* Starting point is exactly on ellipse */

  g = y2 - 1;
  g = g * g * ws + 4 * hs - whs;

  while ((x2 * hs) < (y2 * ws))
    {
      x2 += 2;
      g += hs * 4 * (x2 + 1);

      if (g > 0)		/* Need to drop */
	{
	  y2 -= 2;
	  g -= ws * 4 * y2;
	}

      if ((have_s == 0) && ((s_sin * x2) >= (y2 * s_cos)))
	{
	  pt[0].x = (int) (x2 / 2);
	  pt[0].y = (int) (y2 / 2);
	  have_s = 1;
	}

      if ((have_e == 0) && ((e_sin * x2) >= (y2 * e_cos)))
	{
	  pt[2].x = (int) (x2 / 2);
	  pt[2].y = (int) (y2 / 2);
	  have_e = 1;
	}
    }

  if ((have_s == 0) || (have_e == 0))
    return;			/* Bizarre case */

  if (style == gdPie)
    {
      pt[3] = pt[0];
      pt[4] = pt[1];
      pt[5] = pt[2];

      pt[0].x = cx + (flip_x ? (-pt[0].x) : pt[0].x);
      pt[0].y = cy + (flip_y ? (-pt[0].y) : pt[0].y);
      pt[1].x = cx;
      pt[1].y = cy;
      pt[2].x = cx + (flip_x ? (-pt[2].x) : pt[2].x);
      pt[2].y = cy + (flip_y ? (-pt[2].y) : pt[2].y);
      gdImageFilledPolygon (im, pt, 3, color);
      gdImagePolygon (im, pt, 3, color);

      pt[0] = pt[3];
      pt[1] = pt[4];
      pt[2] = pt[5];
    }

  if (((s_cos * hs) > (s_sin * ws)) && ((e_cos * hs) < (e_sin * ws)))
    {				/* the points are on different parts of the curve...
				 * this is too tricky to try to handle, so divide and conquer:
				 */
      pt[3] = pt[0];
      pt[4] = pt[1];
      pt[5] = pt[2];

      pt[0].x = cx + (flip_x ? (-pt[0].x) : pt[0].x);
      pt[0].y = cy + (flip_y ? (-pt[0].y) : pt[0].y);
      pt[1].x = cx + (flip_x ? (-pt[1].x) : pt[1].x);
      pt[1].y = cy + (flip_y ? (-pt[1].y) : pt[1].y);
      pt[2].x = cx + (flip_x ? (-pt[2].x) : pt[2].x);
      pt[2].y = cy + (flip_y ? (-pt[2].y) : pt[2].y);
      gdImageFilledPolygon (im, pt, 3, color);
      gdImagePolygon (im, pt, 3, color);

      pt[0] = pt[3];
      pt[2] = pt[4];

      conquer = 1;
    }

  if (conquer || (((s_cos * hs) > (s_sin * ws)) && ((e_cos * hs) > (e_sin * ws))))
    {				/* This is the best bit... */
      /* steep line + ellipse */
      /* go up & left from pt[0] to pt[2] */

      x2 = w;
      y2 = 0;			/* Starting point is exactly on ellipse */

      g = x2 - 1;
      g = g * g * hs + 4 * ws - whs;

      while ((x2 * hs) > (y2 * ws))	/* Keep |tangent| > 1 */
	{
	  if ((s_sin * x2) <= (y2 * s_cos))
	    break;

	  y2 += 2;
	  g += ws * 4 * (y2 + 1);

	  if (g > 0)		/* Need to drop */
	    {
	      x2 -= 2;
	      g -= hs * 4 * x2;
	    }
	}

      lx2 = x2;
      ly2 = y2;

      lg = lx2 * (pt[0].y - pt[2].y) - ly2 * (pt[0].x - pt[2].x);
      lg = (lx2 - 1) * (pt[0].y - pt[2].y) - (ly2 + 2) * (pt[0].x - pt[2].x) - lg;

      while (y2 < (2 * pt[2].y))
	{
	  y2 += 2;
	  g += ws * 4 * (y2 + 1);

	  if (g > 0)		/* Need to drop */
	    {
	      x2 -= 2;
	      g -= hs * 4 * x2;
	    }

	  ly2 += 2;
	  lg -= 2 * (pt[0].x - pt[2].x);

	  if (lg < 0)		/* Need to drop */
	    {
	      lx2 -= 2;
	      lg -= 2 * (pt[0].y - pt[2].y);
	    }

	  y = (int) (y2 / 2);
	  for (x = (int) (lx2 / 2); x <= (int) (x2 / 2); x++)
	    {
	      gdImageSetPixel (im, ((flip_x) ? (cx - x) : (cx + x)),
			       ((flip_y) ? (cy - y) : (cy + y)), color);
	    }
	}
    }
  if (conquer)
    {
      pt[0] = pt[4];
      pt[2] = pt[5];
    }
  if (conquer || (((s_cos * hs) < (s_sin * ws)) && ((e_cos * hs) < (e_sin * ws))))
    {				/* This is the best bit... */
      /* gradual line + ellipse */
      /* go down & right from pt[2] to pt[0] */

      x2 = 0;
      y2 = h;			/* Starting point is exactly on ellipse */

      g = y2 - 1;
      g = g * g * ws + 4 * hs - whs;

      while ((x2 * hs) < (y2 * ws))
	{
	  x2 += 2;
	  g += hs * 4 * (x2 + 1);

	  if (g > 0)		/* Need to drop */
	    {
	      y2 -= 2;
	      g -= ws * 4 * y2;
	    }

	  if ((e_sin * x2) >= (y2 * e_cos))
	    break;
	}

      lx2 = x2;
      ly2 = y2;

      lg = lx2 * (pt[0].y - pt[2].y) - ly2 * (pt[0].x - pt[2].x);
      lg = (lx2 + 2) * (pt[0].y - pt[2].y) - (ly2 - 1) * (pt[0].x - pt[2].x) - lg;

      while (x2 < (2 * pt[0].x))
	{
	  x2 += 2;
	  g += hs * 4 * (x2 + 1);

	  if (g > 0)		/* Need to drop */
	    {
	      y2 -= 2;
	      g -= ws * 4 * y2;
	    }

	  lx2 += 2;
	  lg += 2 * (pt[0].y - pt[2].y);

	  if (lg < 0)		/* Need to drop */
	    {
	      ly2 -= 2;
	      lg += 2 * (pt[0].x - pt[2].x);
	    }

	  x = (int) (x2 / 2);
	  for (y = (int) (ly2 / 2); y <= (int) (y2 / 2); y++)
	    {
	      gdImageSetPixel (im, ((flip_x) ? (cx - x) : (cx + x)),
			       ((flip_y) ? (cy - y) : (cy + y)), color);
	    }
	}
    }
}

static gdPoint
gdArcClosest (int width, int height, int angle)
{
  gdPoint pt;

  int flip_x = 0;
  int flip_y = 0;

  long a_sin = 0;
  long a_cos = 0;

  long w;			/* a * 2 */
  long h;			/* b * 2 */

  long x2;			/* x * 2 */
  long y2;			/* y * 2 */

  long ws;			/* (a * 2)^2 */
  long hs;			/* (b * 2)^2 */

  long whs;			/* (a * 2)^2 * (b * 2)^2 */

  long g;			/* decision variable */

  w = (long) ((width & 1) ? (width + 1) : (width));
  h = (long) ((height & 1) ? (height + 1) : (height));

  while (angle < 0)
    angle += 360;
  while (angle >= 360)
    angle -= 360;

  if (angle == 0)
    {
      pt.x = w / 2;
      pt.y = 0;
      return (pt);
    }
  if (angle == 90)
    {
      pt.x = 0;
      pt.y = h / 2;
      return (pt);
    }
  if (angle == 180)
    {
      pt.x = -w / 2;
      pt.y = 0;
      return (pt);
    }
  if (angle == 270)
    {
      pt.x = 0;
      pt.y = -h / 2;
      return (pt);
    }

  pt.x = 0;
  pt.y = 0;

  if ((angle > 90) && (angle < 180))
    {
      angle = 180 - angle;
      flip_x = 1;
    }
  if ((angle > 180) && (angle < 270))
    {
      angle = angle - 180;
      flip_x = 1;
      flip_y = 1;
    }
  if ((angle > 270) && (angle < 360))
    {
      angle = 360 - angle;
      flip_y = 1;
    }

  a_sin = (long) ((double) 32768 * sin ((double) angle * M_PI / (double) 180));
  a_cos = (long) ((double) 32768 * cos ((double) angle * M_PI / (double) 180));

  ws = w * w;
  hs = h * h;

  whs = 1;
  while ((ws > 32768) || (hs > 32768))
    {
      ws = (ws + 1) / 2;	/* Unfortunate limitations on integers makes */
      hs = (hs + 1) / 2;	/* drawing large  ellipses problematic...    */
      whs *= 2;
    }
  while ((ws * hs) > (0x04000000L / whs))
    {
      ws = (ws + 1) / 2;
      hs = (hs + 1) / 2;
      whs *= 2;
    }
  whs *= ws * hs;

  if ((a_cos * hs) > (a_sin * ws))
    {
      x2 = w;
      y2 = 0;			/* Starting point is exactly on ellipse */

      g = x2 - 1;
      g = g * g * hs + 4 * ws - whs;

      while ((x2 * hs) > (y2 * ws))	/* Keep |tangent| > 1 */
	{
	  y2 += 2;
	  g += ws * 4 * (y2 + 1);

	  if (g > 0)		/* Need to drop */
	    {
	      x2 -= 2;
	      g -= hs * 4 * x2;
	    }

	  if ((a_sin * x2) <= (y2 * a_cos))
	    {
	      pt.x = (int) (x2 / 2);
	      pt.y = (int) (y2 / 2);
	      break;
	    }
	}
    }
  else
    {
      x2 = 0;
      y2 = h;			/* Starting point is exactly on ellipse */

      g = y2 - 1;
      g = g * g * ws + 4 * hs - whs;

      while ((x2 * hs) < (y2 * ws))
	{
	  x2 += 2;
	  g += hs * 4 * (x2 + 1);

	  if (g > 0)		/* Need to drop */
	    {
	      y2 -= 2;
	      g -= ws * 4 * y2;
	    }

	  if ((a_sin * x2) >= (y2 * a_cos))
	    {
	      pt.x = (int) (x2 / 2);
	      pt.y = (int) (y2 / 2);
	      break;
	    }
	}
    }

  if (flip_x)
    pt.x = -pt.x;
  if (flip_y)
    pt.y = -pt.y;

  return (pt);
}

#include "gd.h"
#include <string.h>
#include <math.h>

#define WIDTH	500
#define HEIGHT	300

int 
main (int argc, char *argv[])
{
  gdImagePtr im = gdImageCreate (WIDTH, HEIGHT);
  int white = gdImageColorResolve (im, 0xFF, 0xFF, 0xFF), black = gdImageColorResolve (im, 0, 0, 0),
    red = gdImageColorResolve (im, 0xFF, 0xA0, 0xA0);
  FILE *out;

  /* filled arc - circle */
  gdImageFilledArc (im, WIDTH / 5, HEIGHT / 4, 200, 200, 45, 90, red, gdPie);
  gdImageArc (im, WIDTH / 5, HEIGHT / 4, 200, 200, 45, 90, black);

  /* filled arc - ellipse */
  gdImageFilledArc (im, WIDTH / 2, HEIGHT / 4, 200, 150, 45, 90, red, gdPie);
  gdImageArc (im, WIDTH / 2, HEIGHT / 4, 200, 150, 45, 90, black);


  /* reference lines */
  gdImageLine (im, 0, HEIGHT / 4, WIDTH, HEIGHT / 4, black);
  gdImageLine (im, WIDTH / 5, 0, WIDTH / 5, HEIGHT, black);
  gdImageLine (im, WIDTH / 2, 0, WIDTH / 2, HEIGHT, black);
  gdImageLine (im, WIDTH / 2, HEIGHT / 4, WIDTH / 2 + 300, HEIGHT / 4 + 300, black);
  gdImageLine (im, WIDTH / 5, HEIGHT / 4, WIDTH / 5 + 300, HEIGHT / 4 + 300, black);

  /* TBB: Write img to test/arctest.png */
  out = fopen ("test/arctest.png", "wb");
  if (!out)
    {
      php_gd_error("Can't create test/arctest.png\n");
      exit (1);
    }
  gdImagePng (im, out);
  fclose (out);
  php_gd_error("Test image written to test/arctest.png\n");
  /* Destroy it */
  gdImageDestroy (im);

  return 0;
}

#endif
