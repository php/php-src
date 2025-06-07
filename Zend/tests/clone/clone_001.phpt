--TEST--
Using clone statement on non-object
--FILE--
<?php

$a = clone array();

?>
--EXPECTF--
Fatal error: Uncaught TypeError: clone(): Argument #1 ($object) must be of type object, array given in %s:%d
Stack trace:
#0 %s(%d): clone(Array)
#1 {main}
  thrown in %s on line %d
