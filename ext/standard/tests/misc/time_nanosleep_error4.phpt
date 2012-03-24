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

$nano = time_nanosleep(0, -10);

?>
--EXPECTF--
Warning: time_nanosleep(): The nanoseconds value must be greater than 0 in %stime_nanosleep_error4.php on line %d
