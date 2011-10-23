--TEST--
time_sleep_until() function - basic test for time_sleep_until()
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (!function_exists("time_sleep_until")) die('skip time_sleep_until() not available');
?>
--CREDITS--
Manuel Baldassarri mb@ideato.it
Michele Orselli mo@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php
  $time = microtime(true) + 2;
  var_dump(time_sleep_until( (int)$time ));
  var_dump(microtime(true) >= (int)$time);
?>
--EXPECT--
bool(true)
bool(true)
