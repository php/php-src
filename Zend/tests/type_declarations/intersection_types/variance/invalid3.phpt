--TEST--
Replacing int type with intersection type
--FILE--
<?php

interface X {}
interface Y {}

class Test {
    function method(): int {}
}
class Test2 extends Test {
    function method(): X&Y {}
}

?>
--EXPECTF--
Fatal error: Declaration of Test2::method(): X&Y must be compatible with Test::method(): int in %s on line %d
