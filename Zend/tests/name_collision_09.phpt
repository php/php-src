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
--EXPECTF--
Fatal error: Cannot declare const Bazzle\BAR because the name is already in use in %s on line %d
