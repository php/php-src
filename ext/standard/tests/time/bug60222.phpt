--TEST--
Bug #60222 (time_nanosleep() does validate input params)
--FILE--
<?php
	var_dump(time_nanosleep(-1, 0));
	var_dump(time_nanosleep(0, -1));
?>
===DONE===
--EXPECTF--
Warning: time_nanosleep(): The seconds value must be greater than 0 in %s on line %d
bool(false)

Warning: time_nanosleep(): The nanoseconds value must be greater than 0 in %s on line %d
bool(false)
===DONE===
