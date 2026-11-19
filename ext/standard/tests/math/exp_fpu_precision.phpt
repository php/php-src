--TEST--
exp(): results must not lose precision when the FPU is clamped to double precision
--INI--
serialize_precision=-1
--FILE--
<?php
/* On i386 the x87 FPU is the platform default and zend_init_fpu() clamps it to
 * double precision (53-bit mantissa) for the whole request. libm's exp()
 * relies on the FPU running at extended precision internally, so while that
 * clamp is in place its result is off by one or more ULP compared to platforms
 * whose FPU has no precision control.
 *
 * Each expected value below is the correctly rounded double, printed as the
 * shortest decimal that round-trips back to it. */

$x = 3;

echo "-- runtime calls --\n";
var_dump(exp($x));
var_dump(exp($x + 2));
var_dump(exp($x + 3));
var_dump(exp($x + 4));
var_dump(exp($x + 5));
var_dump(exp($x + 6));
var_dump(exp($x + 7));
var_dump(exp($x + 9));
var_dump(exp($x + 12));
var_dump(exp($x + 17));

echo "-- constant argument gives the same result --\n";
var_dump(exp(10) === exp($x + 7));
var_dump(exp(20) === exp($x + 17));
?>
--EXPECT--
-- runtime calls --
float(20.085536923187668)
float(148.4131591025766)
float(403.4287934927351)
float(1096.6331584284585)
float(2980.9579870417283)
float(8103.083927575384)
float(22026.465794806718)
float(162754.79141900392)
float(3269017.3724721107)
float(485165195.4097903)
-- constant argument gives the same result --
bool(true)
bool(true)
