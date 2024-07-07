--TEST--
Tests that a static class cannot inherit an instance property from a trait
--FILE--
<?php

trait T {
    var $V;
}

static class C {
    use T;
}
?>
--EXPECTF--
Fatal error: Static class C cannot use trait with a non-static property T::$V in %s on line %d
