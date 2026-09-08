--TEST--
Using "_" as a constant name is deprecated
--FILE--
<?php

namespace Foo\Bar {
    const _ = 5;
}

namespace {
    const _ = 5;
}

?>
--EXPECTF--
Deprecated: Calling a constant "_" is deprecated since 8.6 in %s on line %d

Deprecated: Calling a constant "_" is deprecated since 8.6 in %s on line %d
