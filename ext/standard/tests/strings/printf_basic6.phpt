--TEST--
Test printf() function : basic functionality - exponential format
--FILE--
<?php
/* Prototype  : int printf  ( string $format  [, mixed $args  [, mixed $...  ]] )
 * Description: Produces output according to format .
 * Source code: ext/standard/formatted_print.c
 */

echo "*** Testing printf() : basic functionality - using exponential format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%e";
$format2 = "%E %e";
$format3 = "%e %E %e";
$arg1 = 1000;
$arg2 = 2e3;
$arg3 = +3e3;

echo "\n-- Calling printf() with no arguments --\n";
$result = printf($format);
echo "\n";
var_dump($result);

echo "\n-- Calling printf() with one argument --\n";
$result = printf($format1, $arg1);
echo "\n";
var_dump($result);

echo "\n-- Calling printf() with two arguments --\n";
$result = printf($format2, $arg1, $arg2);
echo "\n";
var_dump($result);

echo "\n-- Calling printf() with three arguments --\n";
$result = printf($format3, $arg1, $arg2, $arg3);
echo "\n";
var_dump($result);
?>
===DONE===
--EXPECTF--
*** Testing printf() : basic functionality - using exponential format ***

-- Calling printf() with no arguments --
format
int(6)

-- Calling printf() with one argument --
1.000000e+3
int(11)

-- Calling printf() with two arguments --
1.000000E+3 2.000000e+3
int(23)

-- Calling printf() with three arguments --
1.000000e+3 2.000000E+3 3.000000e+3
int(35)
===DONE===
