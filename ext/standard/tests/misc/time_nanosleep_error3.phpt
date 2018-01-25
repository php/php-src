--TEST--
time_nanosleep — Delay for a number of seconds and nanoseconds
--SKIPIF--
<?php
if (strpos(strtoupper(PHP_OS), 'WIN') !== false) die("skip Test is not valid for Windows");
if (!function_exists('time_nanosleep')) die("skip");
?>
--CREDITS--
Àlex Corretgé - alex@corretge.cat
--FILE--
<?php

$nano = time_nanosleep(-2, 1000);

?>
--EXPECTF--
Warning: time_nanosleep(): The seconds value must be greater than 0 in %stime_nanosleep_error3.php on line %d
