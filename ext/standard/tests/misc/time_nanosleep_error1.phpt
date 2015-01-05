--TEST--
time_nanosleep — Delay for a number of seconds and nanoseconds
--SKIPIF--
<?php if (!function_exists('time_nanosleep')) die("skip"); ?>
--CREDITS--
Àlex Corretgé - alex@corretge.cat
--FILE--
<?php

$nano = time_nanosleep('A', 100000);

?>
--EXPECTF--
Warning: time_nanosleep() expects parameter 1 to be integer, %unicode_string_optional% given in %s.php on line %d
