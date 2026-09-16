--TEST--
Using "readonly" as a function name readonly is deprecated
--FILE--
<?php

namespace Foo\Bar {
    function readonly() {}
}

namespace {
    function readonly() {}
}

?>
--EXPECTF--
Deprecated: Calling a function "readonly" is deprecated since 8.6 in %s on line %d

Deprecated: Calling a function "readonly" is deprecated since 8.6 in %s on line %d
