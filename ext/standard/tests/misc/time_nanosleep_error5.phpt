--TEST--
time_nanosleep — Delay for a number of seconds and nanoseconds
--SKIPIF--
<?php if (!function_exists('time_nanosleep')) die("skip"); ?>
--CREDITS--
Àlex Corretgé - alex@corretge.cat
--FILE--
<?php

time_nanosleep(0, 1000000000);

?>
--EXPECTF--
Fatal error: Uncaught ValueError: time_nanosleep(): Argument #2 ($nanoseconds) must be less than or equal to 999 999 999 in %s:%d
Stack trace:
#0 %s(%d): time_nanosleep(0, 1000000000)
#1 {main}
  thrown in %s on line %d
