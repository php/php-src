--TEST--
Tests that a static class cannot inherit an instance method from a trait
--FILE--
<?php

trait T {
    function F() {}
}

static class C {
    use T;
}
?>
--EXPECTF--
Fatal error: Static class C cannot use trait with a non-static method T::F in %s on line %d
