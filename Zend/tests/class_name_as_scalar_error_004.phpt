--TEST--
class name as scalar from ::class keyword error using parent in method signature
--FILE--
<?php

namespace Foo\Bar {
    class One {
        public function baz($x = parent::class) {}
    }
}
?>
--EXPECTF--
Deprecated: Cannot use "parent" without a parent class in %s on line %d

Fatal error: Cannot use "parent" without a parent class in %s on line %d
