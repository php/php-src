--TEST--
Trying to use an abstract class as trait
--FILE--
<?php

abstract class abc {
}

class A {
    use abc;
}

?>
--EXPECTF--
Fatal error: Uncaught Error: A cannot use abc - it is not a trait in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
