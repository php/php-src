--TEST--
The ** operator must not lose precision when the FPU is clamped to double precision
--INI--
serialize_precision=-1
--FILE--
<?php
/* On i386 the x87 FPU is the platform default and zend_init_fpu() clamps it to
 * double precision (53-bit mantissa) for the whole request. libm's pow()
 * computes on the x87 unit and relies on the extended range to round correctly
 * to double, so while that clamp is in place its result is off by one or more
 * ULP.
 *
 * This file covers the ** operator (ZEND_POW). The same cases are covered for
 * pow() in ext/standard/tests/math/pow_fpu_precision.phpt.
 *
 * Every expected value is exactly the correctly rounded double of its decimal
 * literal, so a build that keeps full precision prints the short literal back
 * and compares identical to it. */

$ten = 10;
$five = 5;
$two = 2;

echo "-- runtime --\n";
var_dump($ten ** -2);
var_dump($ten ** -3);
var_dump($ten ** -4);
var_dump($five ** -2);
var_dump($five ** -3);
var_dump($five ** -4);
echo "-- compile-time constant folding --\n";
var_dump(10 ** -2);
var_dump(10 ** -3);
var_dump(10 ** -4);
var_dump(5 ** -2);
var_dump(5 ** -3);
var_dump(5 ** -4);
echo "-- exact powers of two are unaffected --\n";
var_dump($two ** -1);
var_dump($two ** -10);
echo "-- identical to the decimal literal --\n";
var_dump($ten ** -2 === 0.01);
var_dump($ten ** -3 === 0.001);
var_dump($ten ** -4 === 0.0001);
var_dump($five ** -2 === 0.04);
var_dump($five ** -3 === 0.008);
var_dump($five ** -4 === 0.0016);
?>
--EXPECT--
-- runtime --
float(0.01)
float(0.001)
float(0.0001)
float(0.04)
float(0.008)
float(0.0016)
-- compile-time constant folding --
float(0.01)
float(0.001)
float(0.0001)
float(0.04)
float(0.008)
float(0.0016)
-- exact powers of two are unaffected --
float(0.5)
float(0.0009765625)
-- identical to the decimal literal --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
