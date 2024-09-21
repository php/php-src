--TEST--
Using "_" as a enum name is deprecated
--FILE--
<?php

namespace Foo\Bar {
    enum _ {}
}

namespace {
    enum _ {}
}

?>
--EXPECTF--
Deprecated: Using "_" as an enum name is deprecated since 8.4 in %s on line %d

Deprecated: Using "_" as an enum name is deprecated since 8.4 in %s on line %d
