--TEST--
Using "_" as a constant in use statements is deprecated
--FILE--
<?php

namespace Foo\Bar {
    use const PHP_INT_MAX as _;
}

namespace {
    use const PHP_INT_MAX as _;
}

?>
--EXPECTF--
Deprecated: Using "_" as a constant name is deprecated in %s on line %d

Deprecated: Using "_" as a constant name is deprecated in %s on line %d
