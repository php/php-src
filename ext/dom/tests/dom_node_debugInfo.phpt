--TEST--
DOMNode custom __debugInfo() should abort at the first exception
--EXTENSIONS--
dom
--FILE--
<?php

$d = new DOMNode;
var_dump($d);

?>
--EXPECTF--
Fatal error: Uncaught DOMException: Invalid State Error in %s:%d
Stack trace:
#0 %s(%d): var_dump(Object(DOMNode))
#1 {main}
  thrown in %s on line %d
