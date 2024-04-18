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
?>
--EXPECTF--
Fatal error: Cannot redeclare Bazzle\bar() (previously declared as local import) in %s on line %d
