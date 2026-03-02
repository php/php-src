--TEST--
Unknown class in absolute trait precedence reference
--FILE--
<?php

trait T {}
class C {
    use T {
        WrongClass::method insteadof C;
    }
}

?>
--EXPECTF--
Fatal error: Could not find trait WrongClass in %s on line %d
