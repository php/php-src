--TEST--
Using "is" as a class name is deprecated
--FILE--
<?php

namespace Foo\Bar {
    class is {}
}

namespace {
    class is {}
}

?>
--EXPECTF--
Deprecated: Using "is" as a class name is deprecated since 8.6 in %s on line %d

Deprecated: Using "is" as a class name is deprecated since 8.6 in %s on line %d
