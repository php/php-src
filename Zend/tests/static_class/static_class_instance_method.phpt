--TEST--
Tests that a static class cannot contain an instance method
--FILE--
<?php

static class C {
    function F() {}
}
?>
--EXPECTF--
Fatal error: Class method C::F() must be declared static in a static class in %s on line %d
