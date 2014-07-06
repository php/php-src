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
Fatal error: parent::class cannot be used for compile-time class name resolution in %s on line %d
