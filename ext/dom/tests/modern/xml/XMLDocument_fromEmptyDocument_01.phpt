--TEST--
DOM\XMLDocument::createEmpty 01
--EXTENSIONS--
dom
--FILE--
<?php

DOM\XMLDocument::createEmpty(encoding: "foo");

?>
--EXPECTF--
Fatal error: Uncaught ValueError: DOM\XMLDocument::createEmpty(): Argument #2 ($encoding) is not a valid document encoding in %s:%d
Stack trace:
#0 %s(%d): DOM\XMLDocument::createEmpty('1.0', 'foo')
#1 {main}
  thrown in %s on line %d
