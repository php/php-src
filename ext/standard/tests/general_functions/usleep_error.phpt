--TEST--
Test usleep() function : error conditions
--FILE--
<?php
/* Prototype  : void usleep  ( int $micro_seconds  )
 * Description: Delays program execution for the given number of micro seconds.
 * Source code: ext/standard/basic_functions.c
 */

set_time_limit(20);

echo "*** Testing usleep() : error conditions ***\n";

echo "\n-- Testing usleep() function with negative interval --\n";
$seconds = -10;
var_dump( usleep($seconds) );

?>
===DONE===
--EXPECTF--
*** Testing usleep() : error conditions ***

-- Testing usleep() function with negative interval --

Warning: usleep(): Number of microseconds must be greater than or equal to 0 in %s on line %d
bool(false)
===DONE===
