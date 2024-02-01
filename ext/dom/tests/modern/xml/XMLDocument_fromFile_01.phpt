--TEST--
DOM\XMLDocument::createFromFile 01
--EXTENSIONS--
dom
--FILE--
<?php

DOM\XMLDocument::createFromString("");

?>
--EXPECTF--
Fatal error: Uncaught ValueError: DOM\XMLDocument::createFromString(): Argument #1 ($source) must not be empty in %s:%d
Stack trace:
#0 %s(%d): DOM\XMLDocument::createFromString('')
#1 {main}
  thrown in %s on line %d
