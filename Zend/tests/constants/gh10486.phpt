--TEST--
Assertion error when attempting comp-time eval of dynamic class constant fetch
--FILE--
<?php
y::{5}::y;
?>
--EXPECTF--
Fatal error: Uncaught Error: Class "y" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
