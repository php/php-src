--TEST--
Bug #69152: Type Confusion Infoleak Vulnerability in unserialize()
--FILE--
<?php
try {
	$x = unserialize('O:9:"exception":1:{s:16:"'."\0".'Exception'."\0".'trace";s:4:"ryat";}');
	var_dump($x);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
	$x = unserialize('O:4:"test":1:{s:27:"__PHP_Incomplete_Class_Name";R:1;}');
	var_dump($x);
	$x->test();
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Cannot assign string to property Exception::$trace of type array
object(__PHP_Incomplete_Class)#1 (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  *RECURSION*
}
Error: The script tried to call a method on an incomplete object. Please ensure that the class definition "unknown" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition
