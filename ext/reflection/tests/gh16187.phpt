--TEST--
GH-16187 (ReflectionClass::__toString() with unpacked properties)
--EXTENSIONS--
simplexml
--FILE--
<?php
$xml = '<form name="test"></form>';
$simplexml = simplexml_load_string($xml);
$reflector = new ReflectionObject($simplexml['name']);
$reflector->__toString();
?>
DONE
--EXPECT--
DONE
