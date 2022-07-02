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
Fatal error: A cannot use abc - it is not a trait in %s on line %d
