--TEST--
Test strcoll() function : error conditions
--SKIPIF--
<?php if (!function_exists('strcoll')) die('skip strcoll function not available') ?>
--FILE--
<?php
/* Prototype: int strcoll  ( string $str1  , string $str2  )
   Description: Locale based string comparison
*/

echo "*** Testing strcoll() : error conditions ***\n";

echo "\n-- Testing strcoll() function with no arguments --\n";
var_dump( strcoll() );
var_dump( strcoll("") );

echo "\n-- Testing strcoll() function with one argument --\n";
var_dump( strcoll("Hello World") );

echo "\n-- Testing strcoll() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( strcoll("Hello World",  "World", $extra_arg) );

?>
===Done===
--EXPECTF--
*** Testing strcoll() : error conditions ***

-- Testing strcoll() function with no arguments --

Warning: strcoll() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: strcoll() expects exactly 2 parameters, 1 given in %s on line %d
NULL

-- Testing strcoll() function with one argument --

Warning: strcoll() expects exactly 2 parameters, 1 given in %s on line %d
NULL

-- Testing strcoll() function with more than expected no. of arguments --

Warning: strcoll() expects exactly 2 parameters, 3 given in %s on line %d
NULL
===Done===
