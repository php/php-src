--TEST--
Dom\XMLDocument::createEmpty 01
--EXTENSIONS--
dom
--FILE--
<?php

Dom\XMLDocument::createEmpty(encoding: "foo");

?>
--EXPECTF--
Fatal error: Uncaught ValueError: Dom\XMLDocument::createEmpty(): Argument #2 ($encoding) is not a valid document encoding in %s:%d
Stack trace:
#0 %s(%d): Dom\XMLDocument::createEmpty('1.0', 'foo')
#1 {main}
  thrown in %s on line %d
