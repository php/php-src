# The bcmath library

This is a fork of the bcmath library initially created by Phil Nelson in May
2000.

Bcmath is a library of arbitrary precision math routines. These routines, in a
different form, are the routines that to the arbitrary precision calculations
for GNU bc and GNU dc.

This library is provided to make these routines useful in a larger context with
less restrictions on the use of them.

These routines do not duplicate functionality of the GNU gmp library. The gmp
library is similar, but the actual computation is different.

Initial library (version 0.1) has been created in 2000-05-21 and then forked and
bundled into PHP with version 0.2 released in 2000-06-07.

## FAQ

* Why BCMATH?

  The math routines of GNU bc become more generally useful in a library form. By
  separating the BCMATH library from GNU bc, GNU bc can be under the GPL and
  BCMATH can be under the LGPL.

* Why BCMATH when GMP exists?

  GMP has "integers" (no digits after a decimal), "rational numbers" (stored as
  2 integers) and "floats". None of these will correctly represent a POSIX BC
  number. Floats are the closest, but will not behave correctly for many
  computations. For example, BC numbers have a "scale" that represent the number
  of digits to represent after the decimal point. The multiplying two of these
  numbers requires one to calculate an exact number of digits after the decimal
  point regardless of the number of digits in the integer part. GMP floats have
  a "fixed, but arbitrary" mantissa and so multiplying two floats will end up
  dropping digits BC must calculate.

## Credits

Phil Nelson (philnelson@acm.org) wrote bcmath library.

## License

The bcmath library is released under the GNU Lesser General Public License v2.1.
