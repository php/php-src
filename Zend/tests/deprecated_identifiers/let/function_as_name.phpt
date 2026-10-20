--TEST--
Using "let" as a function name is deprecated
--FILE--
<?php

namespace Foo\Bar {
    function let() {}
}

namespace {
    function let() {}
}

?>
--EXPECTF--
Deprecated: Calling a function "let" is deprecated since 8.6 in %s on line %d

Deprecated: Calling a function "let" is deprecated since 8.6 in %s on line %d
