--TEST--
Test gmmktime() function : usage variation - Passing octal and hexadecimal values to arguments.
--FILE--
<?php
/* Prototype  : int gmmktime([int hour [, int min [, int sec [, int mon [, int day [, int year]]]]]])
 * Description: Get UNIX timestamp for a GMT date
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing gmmktime() : usage variation ***\n";

// Initialise all required variables
$hour = 010;
$min = 010;
$sec = 010;
$mon = 010;
$day = 010;
$year = 03730;

echo "\n-- Testing gmmktime() function with supplying octal values to arguments --\n";
var_dump( gmmktime($hour, $min, $sec, $mon, $day, $year) );

// Initialise all required variables
$hour = 0x8;
$min = 0x8;
$sec = 0x8;
$mon = 0x8;
$day = 0x8;
$year = 0x7D8;

echo "\n-- Testing gmmktime() function with supplying hexa decimal values to arguments --\n";
var_dump( gmmktime($hour, $min, $sec, $mon, $day, $year) );
?>
===DONE===
--EXPECT--
*** Testing gmmktime() : usage variation ***

-- Testing gmmktime() function with supplying octal values to arguments --
int(1218182888)

-- Testing gmmktime() function with supplying hexa decimal values to arguments --
int(1218182888)
===DONE===
