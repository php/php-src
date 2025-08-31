--TEST--
Replacing not-loadable parent intersection type with loadable child intersection type
--FILE--
<?php

// Let Y and Z be loadable.
interface Y {}
interface Z {}

class Test {
    function method(): X&Y {}
}
class Test2 extends Test {
    function method(): Y&Z {}
}

?>
===DONE===
--EXPECTF--
Fatal error: Could not check compatibility between Test2::method(): Y&Z and Test::method(): X&Y, because class X is not available in %s on line %d
