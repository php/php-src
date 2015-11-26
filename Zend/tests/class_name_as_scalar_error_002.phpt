--TEST--
class name as scalar from ::class keyword error using parent in class constant
--FILE--
<?php

namespace Foo\Bar {
    class One {
        const Baz = parent::class;
    }
}
?>
--EXPECTF--
Fatal error: parent::class cannot be used for compile-time class name resolution in %s on line %d
