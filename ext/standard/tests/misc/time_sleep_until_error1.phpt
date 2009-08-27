--TEST--
time_sleep_until() function - error test for time_sleep_until()
--SKIPIF--
<?php
	function_exists('time_sleep_until') or die('skip time_sleep_until() is not supported in this build.');
?>
--CREDITS--
Fabio Fabbrucci fabbrucci@grupporetina.com
Danilo Sanchi sanchi@grupporetina.com
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php
  var_dump(time_sleep_until(time()-1));
?>
===DONE===
--EXPECTF--
Warning: time_sleep_until(): Sleep until to time is less than current time in %s on line 2
bool(false)
===DONE===
