--TEST--
GH-12237 (printf/sprintf should not produce negative zero)
--FILE--
<?php

// Basic case from the bug report
echo "-- Basic case --\n";
var_dump(sprintf('%.2f', -0.001));
var_dump(sprintf('%.2f', -0.0));

// Various precisions where the value rounds to zero
echo "-- Precisions rounding to zero --\n";
var_dump(sprintf('%.0f', -0.1));
var_dump(sprintf('%.0f', -0.0));
var_dump(sprintf('%.1f', -0.04));
var_dump(sprintf('%.1f', -0.001));
var_dump(sprintf('%.2f', -0.004));
var_dump(sprintf('%.2f', -0.001));
var_dump(sprintf('%.3f', -0.0004));
var_dump(sprintf('%.3f', -0.0001));
var_dump(sprintf('%.4f', -0.00004));
var_dump(sprintf('%.4f', -0.00001));

// Values that should remain negative (do NOT round to zero)
echo "-- Should stay negative --\n";
var_dump(sprintf('%.2f', -0.01));
var_dump(sprintf('%.2f', -0.1));
var_dump(sprintf('%.2f', -1.5));
var_dump(sprintf('%.1f', -0.1));
var_dump(sprintf('%.0f', -1.0));

// Uppercase F format
echo "-- Uppercase F --\n";
var_dump(sprintf('%.2F', -0.001));
var_dump(sprintf('%.2F', -0.01));

// Explicit positive sign should show + when value rounds to zero
echo "-- Explicit sign --\n";
var_dump(sprintf('%+.2f', -0.001));
var_dump(sprintf('%+.2f', 0.001));
var_dump(sprintf('%+.2f', -0.01));

// printf output (not just sprintf)
echo "-- printf --\n";
printf('%.2f', -0.001);
echo "\n";
printf('%.2f', -0.01);
echo "\n";

// Consistency with number_format
echo "-- number_format consistency --\n";
var_dump(number_format(-0.001, 2));
var_dump(number_format(-0.01, 2));

// Very small negative values that round to zero
echo "-- Very small negatives --\n";
var_dump(sprintf('%.2f', -1e-10));
var_dump(sprintf('%.2f', -PHP_FLOAT_EPSILON));
var_dump(sprintf('%.2f', -1e-100));

// Padding and width
echo "-- Padding --\n";
var_dump(sprintf('%08.2f', -0.001));
var_dump(sprintf('%08.2f', -0.01));

?>
--EXPECT--
-- Basic case --
string(4) "0.00"
string(4) "0.00"
-- Precisions rounding to zero --
string(1) "0"
string(1) "0"
string(3) "0.0"
string(3) "0.0"
string(4) "0.00"
string(4) "0.00"
string(5) "0.000"
string(5) "0.000"
string(6) "0.0000"
string(6) "0.0000"
-- Should stay negative --
string(5) "-0.01"
string(5) "-0.10"
string(5) "-1.50"
string(4) "-0.1"
string(2) "-1"
-- Uppercase F --
string(4) "0.00"
string(5) "-0.01"
-- Explicit sign --
string(5) "+0.00"
string(5) "+0.00"
string(5) "-0.01"
-- printf --
0.00
-0.01
-- number_format consistency --
string(4) "0.00"
string(5) "-0.01"
-- Very small negatives --
string(4) "0.00"
string(4) "0.00"
string(4) "0.00"
-- Padding --
string(8) "00000.00"
string(8) "-0000.01"
