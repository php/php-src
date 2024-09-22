--TEST--
Using "_" as a trait name is deprecated
--FILE--
<?php

namespace Foo\Bar {
    trait _ {}
}

namespace {
    trait _ {}
}

?>
--EXPECTF--
Deprecated: Using "_" as a trait name is deprecated since 8.4 in %s on line %d

Deprecated: Using "_" as a trait name is deprecated since 8.4 in %s on line %d
