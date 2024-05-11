--TEST--
Getting the address of an uninitialized property of a SimpleXMLElement
--EXTENSIONS--
simplexml
--FILE--
<?php

$rc = new ReflectionClass('SimpleXMLElement');
$sxe = $rc->newInstanceWithoutConstructor();
$sxe->a['b'] = 'b';

?>
--EXPECTF--
Fatal error: Uncaught Error: SimpleXMLElement is not properly initialized in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
