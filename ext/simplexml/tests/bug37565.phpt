--TEST--
Bug #37565 (Using reflection::export with simplexml causing a crash)
--SKIPIF--
<?php if (!extension_loaded("simplexml") || !extension_loaded('reflection')) print "skip"; ?>
--FILE--
<?php

function my_error_handler($errno, $errstr, $errfile, $errline) {
	    echo "Error: $errstr\n";
}

set_error_handler('my_error_handler');

class Setting extends ReflectionObject
{
}

Reflection::export(simplexml_load_string('<test/>', 'Setting'));

Reflection::export(simplexml_load_file('data:,<test/>', 'Setting'));

?>
===DONE===
--EXPECTF--
Error: simplexml_load_string() expects parameter 2 to be a class name derived from SimpleXMLElement, 'Setting' given
Error: Argument 1 passed to Reflection::export() must implement interface Reflector, null given
Error: Reflection::export() expects parameter 1 to be Reflector, null given
Error: simplexml_load_file() expects parameter 2 to be a class name derived from SimpleXMLElement, 'Setting' given
Error: Argument 1 passed to Reflection::export() must implement interface Reflector, null given
Error: Reflection::export() expects parameter 1 to be Reflector, null given
===DONE===
