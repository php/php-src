--TEST--
fpow(): results must not lose precision when the FPU is clamped to double precision
--INI--
serialize_precision=-1
--FILE--
<?php
/* fpow() calls libm pow() directly instead of going through the ** operator's
 * safe_pow(), so it needs the same extended-precision handling: on i386 PHP
 * clamps the x87 FPU to a 53-bit mantissa for the whole request, which costs
 * pow() one or more ULP. See ext/standard/tests/math/pow_fpu_precision.phpt. */

$ten = 10.0;
$five = 5.0;
$two = 2.0;

echo "-- precision --\n";
var_dump(fpow($ten, -2.0));
var_dump(fpow($ten, -3.0));
var_dump(fpow($ten, -4.0));
var_dump(fpow($five, -2.0));
var_dump(fpow($five, -3.0));
var_dump(fpow($five, -4.0));

echo "-- exact powers of two are unaffected --\n";
var_dump(fpow($two, -1.0));
var_dump(fpow($two, -10.0));

echo "-- identical to the decimal literal --\n";
var_dump(fpow($ten, -2.0) === 0.01);
var_dump(fpow($ten, -3.0) === 0.001);
var_dump(fpow($ten, -4.0) === 0.0001);
var_dump(fpow($five, -2.0) === 0.04);
var_dump(fpow($five, -3.0) === 0.008);
var_dump(fpow($five, -4.0) === 0.0016);

echo "-- matches the ** operator --\n";
var_dump(fpow($ten, -2.0) === $ten ** -2);
var_dump(fpow($five, -4.0) === $five ** -4);

echo "-- IEEE-754 semantics are preserved --\n";
var_dump(fpow(0.0, -1.0));
var_dump(fpow(-0.0, -3.0));
var_dump(fpow(1.0, NAN));
var_dump(fpow(NAN, 0.0));
var_dump(fpow(-1.0, INF));
var_dump(fpow(-8.0, 1 / 3));
?>
--EXPECT--
-- precision --
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
-- matches the ** operator --
bool(true)
bool(true)
-- IEEE-754 semantics are preserved --
float(INF)
float(-INF)
float(1)
float(1)
float(1)
float(NAN)
