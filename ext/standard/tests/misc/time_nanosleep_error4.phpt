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

try {
	time_nanosleep(0, -10);
} catch (ValueError $exception) {
    echo $exception->getMessage(), "\n";
}

?>
--EXPECT--
time_nanosleep(): Argument #2 ($nanoseconds) must be between 0 and 999999999
