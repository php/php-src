--TEST--
Cannot catch "static"
--FILE--
<?php

// This could in principle be supported, but isn't right now.
class Test {
    public function method() {
        try {
            foo();
        } catch (static $e) {}
    }
}

?>
--EXPECTF--
Fatal error: Bad class name in the catch statement in %s on line %d
