--TEST--
Trying to instantiate a trait
--FILE--
<?php

trait abc {
}

new abc;

?>
--EXPECTF--
Fatal error: Uncaught Error: Trait abc cannot be instantiated in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
