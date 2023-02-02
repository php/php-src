--TEST--
Assertion error when attempting constant eval of dynamic class constant fetch
--FILE--
<?php
const y = y::{y};
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "y" in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
