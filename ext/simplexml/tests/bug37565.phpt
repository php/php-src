--TEST--
Bug #37565 Using reflection::export with simplexml causing a crash 
--FILE--
<?php

class Setting extends ReflectionObject
{
}

Reflection::export(simplexml_load_string('<test/>', 'Setting'));

Reflection::export(simplexml_load_file('data:,<test/>', 'Setting'));

?>
===DONE===
--EXPECTF--

Warning: simplexml_load_string() expects parameter 2 to be a class name derived from SimpleXMLElement, 'Setting' given in %sbug37565.php on line %d

Warning: Reflection::export() expects parameter 1 to be Reflector, null given in %sbug37565.php on line %d

Warning: simplexml_load_file() expects parameter 2 to be a class name derived from SimpleXMLElement, 'Setting' given in %sbug37565.php on line %d

Warning: Reflection::export() expects parameter 1 to be Reflector, null given in %sbug37565.php on line %d
===DONE===
