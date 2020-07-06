--TEST--
Function declaration colliding with import (in namespace)
--FILE--
<?php

namespace Foo {
    function bar() {}
}

namespace Bazzle {
    use function Foo\bar;
    function bar() {}
}
--EXPECTF--
Fatal error: Function Bazzle\bar() cannot be declared because the name is already in use in %s on line %d
