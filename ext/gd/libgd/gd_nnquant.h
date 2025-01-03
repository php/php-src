/* maximum number of colours that can be used.
  actual number is now passed to initcolors */
#define MAXNETSIZE	256

/* For 256 colours, fixed arrays need 8kb, plus space for the image
   ---------------------------------------------------------------- */


/* four primes near 500 - assume no image has a length so large */
/* that it is divisible by all four primes */
#define prime1		499
#define prime2		491
#define prime3		487
#define prime4		503

#define minpicturebytes	(4*prime4)		/* minimum size for input image */
