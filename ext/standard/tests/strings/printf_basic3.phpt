--TEST--
Test printf() function : basic functionality - float format
--FILE--
<?php
/* Prototype  : int printf  ( string $format  [, mixed $args  [, mixed $...  ]] )
 * Description: Produces output according to format .
 * Source code: ext/standard/formatted_print.c
 */

echo "*** Testing printf() : basic functionality - using float format ***\n";


// Initialise all required variables

$format = "format";
$format1 = "%f";
$format2 = "%f %f";
$format3 = "%f %f %f";

$format11 = "%F";
$format22 = "%F %F";
$format33 = "%F %F %F";
$arg1 = 11.11;
$arg2 = 22.22;
$arg3 = 33.33;

echo "\n-- Calling printf() with no arguments--\n";
$result = printf($format);
echo "\n";
var_dump($result);

echo "\n-- Calling printf() with one arguments--\n";
$result = printf($format1, $arg1);
echo "\n";
var_dump($result);
$result = printf($format11, $arg1);
echo "\n";
var_dump($result);

echo "\n-- Calling printf() with two arguments--\n";
$result = printf($format2, $arg1, $arg2);
echo "\n";
var_dump($result);
$result = printf($format22, $arg1, $arg2);
echo "\n";
var_dump($result);

echo "\n-- Calling printf() with three arguments--\n";
$result = printf($format3, $arg1, $arg2, $arg3);
echo "\n";
var_dump($result);
$result = printf($format33, $arg1, $arg2, $arg3);
echo "\n";
var_dump($result);

?>
===DONE===
--EXPECT--
*** Testing printf() : basic functionality - using float format ***

-- Calling printf() with no arguments--
format
int(6)

-- Calling printf() with one arguments--
11.110000
int(9)
11.110000
int(9)

-- Calling printf() with two arguments--
11.110000 22.220000
int(19)
11.110000 22.220000
int(19)

-- Calling printf() with three arguments--
11.110000 22.220000 33.330000
int(29)
11.110000 22.220000 33.330000
int(29)
===DONE===
