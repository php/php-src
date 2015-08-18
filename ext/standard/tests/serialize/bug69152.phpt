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
Notice: Undefined property: Exception::$previous in %s on line %d
Exception in %s:%d
Stack trace:
#0 {main}

Fatal error: main(): The script tried to execute a method or access a property of an incomplete object. Please ensure that the class definition "unknown" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide a __autoload() function to load the class definition  in %s on line %d
