--TEST--
Class declaration colliding with import (in namespace)
--FILE--
<?php

namespace Foo {
    const BAR = 42;
}

namespace Bazzle {
    use const Foo\BAR;
    const BAR = 24;
}
?>
--EXPECTF--
Fatal error: Constant Bazzle\BAR has already been declared in %s on line %d
