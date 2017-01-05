--TEST--
time_sleep_until() function - error test for time_sleep_until()
--SKIPIF--
<?php	if (!function_exists("time_sleep_until")) die('skip time_sleep_until() not available');?>
--CREDITS--
Filippo De Santis fd@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php
  var_dump(time_sleep_until('goofy'));
?>
--EXPECTF--
Warning: time_sleep_until() expects parameter 1 to be float, string given in %s on line 2
NULL
