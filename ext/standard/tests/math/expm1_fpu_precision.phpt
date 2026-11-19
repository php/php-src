--TEST--
expm1(): results must not lose precision when the FPU is clamped to double precision
--INI--
serialize_precision=-1
--FILE--
<?php
/* On i386 the x87 FPU is the platform default and zend_init_fpu() clamps it to
 * double precision (53-bit mantissa) for the whole request. libm's expm1()
 * computes on the x87 unit and relies on the extended range to round correctly
 * to double, so while that clamp is in place its result is off by one or more
 * ULP.
 *
 * Every expected value below is the correctly rounded double, verified against
 * a high-precision reference and written as the shortest decimal that
 * round-trips back to it. All arguments are exact binary fractions. */

echo "-- reference points already correct on every platform --\n";
var_dump(expm1(0));
var_dump(expm1(2));
var_dump(expm1(4));
echo "-- inputs the FPU clamp gets wrong --\n";
var_dump(expm1(3));
var_dump(expm1(5));
var_dump(expm1(6));
var_dump(expm1(7));
var_dump(expm1(8));
var_dump(expm1(9));
var_dump(expm1(10));
var_dump(expm1(11));
var_dump(expm1(12));
var_dump(expm1(13));
?>
--EXPECT--
-- reference points already correct on every platform --
float(0)
float(6.38905609893065)
float(53.598150033144236)
-- inputs the FPU clamp gets wrong --
float(19.085536923187668)
float(147.4131591025766)
float(402.4287934927351)
float(1095.6331584284585)
float(2979.9579870417283)
float(8102.083927575384)
float(22025.465794806718)
float(59873.14171519782)
float(162753.79141900392)
float(442412.3920089205)
