--TEST--
Test printf() function : basic functionality - string format
--FILE--
<?php
/* Prototype  : int printf  ( string $format  [, mixed $args  [, mixed $...  ]] )
 * Description: Produces output according to format .
 * Source code: ext/standard/formatted_print.c
 */

echo "*** Testing printf() : basic functionality - using string format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%s";
$format2 = "%s %s";
$format3 = "%s %s %s";
$arg1 = "arg1 argument";
$arg2 = "arg2 argument";
$arg3 = "arg3 argument";

echo "\n-- Calling printf() with no arguments --\n";
$result = printf($format);
echo "\n";
var_dump($result);

echo "\n-- Calling printf() with one arguments --\n";
$result = printf($format1, $arg1);
echo "\n";
var_dump($result);

echo "\n-- Calling printf() with two arguments --\n";
$result = printf($format2, $arg1, $arg2);
echo "\n";
var_dump($result);


echo "\n-- Calling printf() with string three arguments --\n";
$result = printf($format3, $arg1, $arg2, $arg3);
echo "\n";
var_dump($result);

?>
===DONE===
--EXPECTF--
*** Testing printf() : basic functionality - using string format ***

-- Calling printf() with no arguments --
format
int(6)

-- Calling printf() with one arguments --
arg1 argument
int(13)

-- Calling printf() with two arguments --
arg1 argument arg2 argument
int(27)

-- Calling printf() with string three arguments --
arg1 argument arg2 argument arg3 argument
int(41)
===DONE===
