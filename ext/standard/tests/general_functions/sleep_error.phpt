--TEST--
Test sleep() function : error conditions
--FILE--
<?php
/* Prototype  : int sleep  ( int $seconds  )
 * Description: Delays the program execution for the given number of seconds .
 * Source code: ext/standard/basic_functions.c
 */
 set_time_limit(20);

echo "*** Testing sleep() : error conditions ***\n";

echo "\n-- Testing sleep() function with negative interval --\n";
$seconds = -10;
var_dump( sleep($seconds) );

?>
===DONE===
--EXPECTF--
*** Testing sleep() : error conditions ***

-- Testing sleep() function with negative interval --

Warning: sleep(): Number of seconds must be greater than or equal to 0 in %s on line %d
bool(false)
===DONE===
