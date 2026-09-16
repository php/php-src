--TEST--
Using "is" as a class name in use statements is deprecated
--FILE--
<?php

namespace Foo\Bar {
    use stdClass as is;
}

namespace {
    use stdClass as is;
}

?>
--EXPECTF--
Deprecated: Using "is" as a class name is deprecated in %s on line %d

Deprecated: Using "is" as a class name is deprecated in %s on line %d
