--TEST--
Using "is" as a function name is deprecated
--FILE--
<?php

namespace Foo\Bar {
    function is() {}
}

namespace {
    function is() {}
}

?>
--EXPECTF--
Deprecated: Calling a function "is" is deprecated since 8.6 in %s on line %d

Deprecated: Calling a function "is" is deprecated since 8.6 in %s on line %d
