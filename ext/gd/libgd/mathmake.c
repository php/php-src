#include <stdio.h>
#include <math.h>

#define scale 1024

int basis[91];
int cost[360];

main (void)
{
  int i;
  printf ("#define costScale %d\n", scale);
  printf ("int cost[] = {\n  ");
  for (i = 0; (i <= 90); i++)
    {
      basis[i] = cos ((double) i * .0174532925) * scale;
    }
  for (i = 0; (i < 90); i++)
    {
      printf ("%d,\n  ", cost[i] = basis[i]);
    }
  for (i = 90; (i < 180); i++)
    {
      printf ("%d,\n  ", cost[i] = -basis[180 - i]);
    }
  for (i = 180; (i < 270); i++)
    {
      printf ("%d,\n  ", cost[i] = -basis[i - 180]);
    }
  for (i = 270; (i < 359); i++)
    {
      printf ("%d,\n  ", cost[i] = basis[360 - i]);
    }
  printf ("%d\n", cost[359] = basis[1]);
  printf ("};\n");
  printf ("#define sintScale %d\n", scale);
  printf ("int sint[] = {\n  ");
  for (i = 0; (i < 360); i++)
    {
      int val;
      val = cost[(i + 270) % 360];
      if (i != 359)
	{
	  printf ("%d,\n  ", val);
	}
      else
	{
	  printf ("%d\n", val);
	}
    }
  printf ("};\n");
}
