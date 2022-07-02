--TEST--
time_nanosleep — Delay for a number of seconds and nanoseconds
--SKIPIF--
<?php
if (!function_exists('time_nanosleep')) die("skip");
?>
--CREDITS--
Àlex Corretgé - alex@corretge.cat
--FILE--
<?php

time_nanosleep(-2, 1000);

?>
--EXPECTF--
Fatal error: Uncaught ValueError: time_nanosleep(): Argument #1 ($seconds) must be greater than or equal to 0 in %s:%d
Stack trace:
#0 %s(%d): time_nanosleep(-2, 1000)
#1 {main}
  thrown in %s on line %d
