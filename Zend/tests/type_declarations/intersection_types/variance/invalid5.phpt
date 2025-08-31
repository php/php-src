--TEST--
Replacing iterable type with non-Traversable intersection type
--FILE--
<?php

interface X {}
interface Y {}

class Test {
    function method(): iterable {}
}
class Test2 extends Test {
    function method(): X&Y {}
}

?>
--EXPECTF--
Fatal error: Declaration of Test2::method(): X&Y must be compatible with Test::method(): Traversable|array in %s on line %d
