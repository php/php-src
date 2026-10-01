--TEST--
Using "let" as a class name is deprecated
--FILE--
<?php

namespace Foo\Bar {
    class let {}
}

namespace {
    class let {}
}

?>
--EXPECTF--
Deprecated: Using "let" as a class name is deprecated since 8.6 in %s on line %d

Deprecated: Using "let" as a class name is deprecated since 8.6 in %s on line %d
