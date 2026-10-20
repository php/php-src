--TEST--
Using "let" as a constant in use statements is deprecated
--FILE--
<?php

namespace Foo\Bar {
    use const PHP_INT_MAX as let;
}

namespace {
    use const PHP_INT_MAX as let;
}

?>
--EXPECTF--
Deprecated: Using "let" as a constant name is deprecated since 8.6 in %s on line %d

Deprecated: Using "let" as a constant name is deprecated since 8.6 in %s on line %d
