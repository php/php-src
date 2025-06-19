--TEST--
Using clone statement on undefined variable
--FILE--
<?php

$a = clone $b;

?>
--EXPECTF--
Warning: Undefined variable $b in %s on line %d

Fatal error: Uncaught TypeError: clone(): Argument #1 ($object) must be of type object, null given in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
