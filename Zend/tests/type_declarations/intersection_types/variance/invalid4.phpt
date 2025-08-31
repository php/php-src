--TEST--
Replacing object type with not-loadable intersection type
--FILE--
<?php


class Test {
    function method(): object {}
}
class Test2 extends Test {
    function method(): X&Y {}
}

?>
===DONE===
--EXPECTF--
Fatal error: Could not check compatibility between Test2::method(): X&Y and Test::method(): object, because class X is not available in %s on line %d
