--TEST--
time_nanosleep — Delay for a number of seconds and nanoseconds
--SKIPIF--
<?php
if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') die("skip Test is not valid for Windows");
if (!function_exists('time_nanosleep')) die("skip");
?>
--CREDITS--
Àlex Corretgé - alex@corretge.cat
--FILE--
<?php

time_nanosleep(0, -10);

?>
--EXPECTF--
Fatal error: Uncaught ValueError: The nanoseconds value must be greater than 0 in %s:%d
Stack trace:
#0 %s(%d): time_nanosleep(0, -10)
#1 {main}
  thrown in %s on line %d
