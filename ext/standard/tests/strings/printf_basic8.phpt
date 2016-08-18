--TEST--
Test printf() function : basic functionality - octal format
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) {
	    die("skip this test is for 32bit platform only");
}
?>
--FILE--
<?php
/* Prototype  : int printf  ( string $format  [, mixed $args  [, mixed $...  ]] )
 * Description: Produces output according to format .
 * Source code: ext/standard/formatted_print.c
 */

echo "*** Testing printf() : basic functionality - using octal format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%o";
$format2 = "%o %o";
$format3 = "%o %o %o";
$arg1 = 021;
$arg2 = -0347;
$arg3 = 0567;

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

echo "\n-- Calling printf() with three arguments --\n";
$result = printf($format3, $arg1, $arg2, $arg3);
echo "\n";
var_dump($result);

?>
===DONE===
--EXPECTF--
*** Testing printf() : basic functionality - using octal format ***

-- Calling printf() with no arguments --
format
int(6)

-- Calling printf() with one arguments --
21
int(2)

-- Calling printf() with two arguments --
21 37777777431
int(14)

-- Calling printf() with three arguments --
21 37777777431 567
int(18)
===DONE===
