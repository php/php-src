--TEST--
Using clone statement on undefined variable
--FILE--
<?php

$a = clone $b;

?>
--EXPECTF--
Warning: Undefined variable $b (This will become an error in PHP 9.0) in %s on line %d

Fatal error: Uncaught Error: __clone method called on non-object in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
