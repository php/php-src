--TEST--
Bug #37565 (Using reflection::export with simplexml causing a crash)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

function my_error_handler($errno, $errstr, $errfile, $errline) {
	    echo "Error: $errstr\n";
}

set_error_handler('my_error_handler');

class Setting extends ReflectionObject
{
}

try {
	Reflection::export(simplexml_load_string('<test/>', 'Setting'));
} catch (Error $e) {
	my_error_handler($e->getCode(), $e->getMessage(), $e->getFile(), $e->getLine());
}

try {
	Reflection::export(simplexml_load_file('data:,<test/>', 'Setting'));
} catch (Error $e) {
	my_error_handler($e->getCode(), $e->getMessage(), $e->getFile(), $e->getLine());
}

?>
===DONE===
--EXPECT--
Error: simplexml_load_string() expects parameter 2 to be a class name derived from SimpleXMLElement, 'Setting' given
Error: Function Reflection::export() is deprecated
Error: Argument 1 passed to Reflection::export() must implement interface Reflector, null given
Error: simplexml_load_file() expects parameter 2 to be a class name derived from SimpleXMLElement, 'Setting' given
Error: Function Reflection::export() is deprecated
Error: Argument 1 passed to Reflection::export() must implement interface Reflector, null given
===DONE===
