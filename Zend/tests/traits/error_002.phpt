--TEST--
Trying to use an undefined trait
--FILE--
<?php

class A {
    use abc;
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Trait "abc" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
