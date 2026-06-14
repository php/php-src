--TEST--
Trying to use a final class as trait
--FILE--
<?php

final class abc {
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
