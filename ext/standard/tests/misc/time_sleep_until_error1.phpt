--TEST--
time_sleep_until() function - error test for time_sleep_until()
--SKIPIF--
<?php	if (!function_exists("time_sleep_until")) die('skip time_sleep_until() not available');?>
--CREDITS--
Fabio Fabbrucci fabbrucci@grupporetina.com
Danilo Sanchi sanchi@grupporetina.com
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php

var_dump(time_sleep_until(time() -1));

?>
--EXPECTF--
Warning: time_sleep_until(): Argument #1 ($timestamp) must be greater than or equal to the current time in %s on line %d
bool(false)
