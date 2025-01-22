--TEST--
Dom\XMLDocument::createFromFile 01
--EXTENSIONS--
dom
--FILE--
<?php

Dom\XMLDocument::createFromString("");

?>
--EXPECTF--
Fatal error: Uncaught ValueError: Dom\XMLDocument::createFromString(): Argument #1 ($source) must not be empty in %s:%d
Stack trace:
#0 %s(%d): Dom\XMLDocument::createFromString('')
#1 {main}
  thrown in %s on line %d
