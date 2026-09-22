--TEST--
class name as scalar from ::class keyword error using static in method signature
--FILE--
<?php

namespace Foo\Bar {
    class One {
        public function baz($x = static::class) {}
    }
}
?>
--EXPECTF--
Fatal error: static::class cannot be used for compile-time class name resolution in %s on line %d
