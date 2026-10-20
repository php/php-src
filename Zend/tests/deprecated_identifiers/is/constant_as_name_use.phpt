--TEST--
Using "is" as a constant in use statements is deprecated
--FILE--
<?php

namespace Foo\Bar {
    use const PHP_INT_MAX as is;
}

namespace {
    use const PHP_INT_MAX as is;
}

?>
--EXPECTF--
Deprecated: Using "is" as a constant name is deprecated since 8.6 in %s on line %d

Deprecated: Using "is" as a constant name is deprecated since 8.6 in %s on line %d
