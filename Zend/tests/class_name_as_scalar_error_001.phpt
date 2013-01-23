--TEST--
class name as scalar from ::class keyword error using static in class constant
--FILE--
<?php

namespace Foo\Bar {
    class One {
        const Baz = static::class;
    }
}
?>
--EXPECTF--
Fatal error: static::class cannot be used for compile-time class name resolution in %s on line %d
