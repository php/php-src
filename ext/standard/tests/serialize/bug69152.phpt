--TEST--
Bug #69152: Type Confusion Infoleak Vulnerability in unserialize()
--FILE--
<?php
$x = unserialize('O:9:"exception":1:{s:16:"'."\0".'Exception'."\0".'trace";s:4:"ryat";}');
echo $x;
$x =  unserialize('O:4:"test":1:{s:27:"__PHP_Incomplete_Class_Name";R:1;}');
$x->test();

?>
--EXPECTF--
Fatal error: Uncaught TypeError: trace is not an array in %s:%d
%a
  thrown in %s on line %d
