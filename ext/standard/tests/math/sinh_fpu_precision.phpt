--TEST--
sinh(): results must not lose precision when the FPU is clamped to double precision
--INI--
serialize_precision=-1
--FILE--
<?php
/* On i386 the x87 FPU is the platform default and zend_init_fpu() clamps it to
 * double precision (53-bit mantissa) for the whole request. libm's sinh()
 * computes on the x87 unit and relies on the extended range to round correctly
 * to double, so while that clamp is in place its result is off by one or more
 * ULP.
 *
 * Every expected value below is the correctly rounded double, verified against
 * a high-precision reference and written as the shortest decimal that
 * round-trips back to it. All arguments are exact binary fractions. */

echo "-- reference points already correct on every platform --\n";
var_dump(sinh(0));
var_dump(sinh(1));
var_dump(sinh(17));
echo "-- inputs the FPU clamp gets wrong --\n";
var_dump(sinh(3));
var_dump(sinh(5));
var_dump(sinh(6));
var_dump(sinh(7));
var_dump(sinh(8));
var_dump(sinh(9));
var_dump(sinh(10));
var_dump(sinh(11));
var_dump(sinh(13));
var_dump(sinh(14));
?>
--EXPECT--
-- reference points already correct on every platform --
float(0)
float(1.1752011936438014)
float(12077476.376787629)
-- inputs the FPU clamp gets wrong --
float(10.017874927409903)
float(74.20321057778875)
float(201.71315737027922)
float(548.3161232732465)
float(1490.4788257895502)
float(4051.54190208279)
float(11013.232874703393)
float(29937.07084924806)
float(221206.6960033301)
float(601302.1420819727)
