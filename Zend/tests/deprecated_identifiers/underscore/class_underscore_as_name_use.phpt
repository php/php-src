--TEST--
Using "_" as a class name in use statements is deprecated
--FILE--
<?php

namespace Foo\Bar {
    use stdClass as _;
}

namespace {
    use stdClass as _;
}

?>
--EXPECTF--
Deprecated: Using "_" as a class name is deprecated in %s on line %d

Deprecated: Using "_" as a class name is deprecated in %s on line %d
