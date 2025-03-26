--TEST--
nested inside function
--FILE--
<?php

class Outer {
    public function test() {
        class Inner {}
    }
}
?>
--EXPECTF--
Fatal error: Class declarations may not be declared inside functions in %s on line %d
