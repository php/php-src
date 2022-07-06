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
Fatal error: Uncaught ValueError: Nanoseconds was not in the range 0 to 999 999 999 or seconds was negative in %s:%d
Stack trace:
#0 %s(%d): time_nanosleep(0, 1000000000)
#1 {main}
  thrown in %s on line %d
