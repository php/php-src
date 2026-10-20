--TEST--
Using "let" as a function in use statements is deprecated
--FILE--
<?php

namespace Foo\Bar {
    use function strlen as let;
}

namespace {
    use function strlen as let;
}

?>
--EXPECTF--
Deprecated: Using "let" as a function name is deprecated since 8.6 in %s on line %d

Deprecated: Using "let" as a function name is deprecated since 8.6 in %s on line %d
