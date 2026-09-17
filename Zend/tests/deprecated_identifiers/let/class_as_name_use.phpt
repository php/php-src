--TEST--
Using "let" as a class name in use statements is deprecated
--FILE--
<?php

namespace Foo\Bar {
    use stdClass as let;
}

namespace {
    use stdClass as let;
}

?>
--EXPECTF--
Deprecated: Using "let" as a class name is deprecated in %s on line %d

Deprecated: Using "let" as a class name is deprecated in %s on line %d
