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
Deprecated: Cannot use "parent" without a parent class in %s on line %d

Fatal error: Cannot use "parent" without a parent class in %s on line %d
