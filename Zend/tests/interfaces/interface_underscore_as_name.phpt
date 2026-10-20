--TEST--
Using "_" as an interface name is deprecated
--FILE--
<?php

namespace Foo\Bar {
    interface _ {}
}

namespace {
    interface _ {}
}

?>
--EXPECTF--
Deprecated: Using "_" as an interface name is deprecated since 8.4 in %s on line %d

Deprecated: Using "_" as an interface name is deprecated since 8.4 in %s on line %d
