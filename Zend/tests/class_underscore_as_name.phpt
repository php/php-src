--TEST--
Using "_" as a class name is deprecated
--FILE--
<?php

namespace Foo\Bar {
    class _ {}
}

namespace {
    class _ {}
}

?>
--EXPECTF--
Deprecated: Using "_" as a class name is deprecated since 8.4 in %s on line %d

Deprecated: Using "_" as a class name is deprecated since 8.4 in %s on line %d
