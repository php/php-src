--TEST--
Using "is" as a function in use statements is deprecated
--FILE--
<?php

namespace Foo\Bar {
    use function strlen as is;
}

namespace {
    use function strlen as is;
}

?>
--EXPECTF--
Deprecated: Using "is" as a function name is deprecated since 8.6 in %s on line %d

Deprecated: Using "is" as a function name is deprecated since 8.6 in %s on line %d
