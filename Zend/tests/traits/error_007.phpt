--TEST--
Trying to instantiate a trait
--FILE--
<?php

trait abc { 
}

new abc;

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot instantiate trait abc in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
