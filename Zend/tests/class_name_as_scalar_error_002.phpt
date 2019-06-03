--TEST--
class name as scalar from ::class keyword error using parent in class constant
--FILE--
<?php

namespace Foo\Bar {
    class One {
        const Baz = parent::class;
    }
    var_dump(One::Baz);
}
?>
--EXPECTF--
Deprecated: Cannot use "parent" when current class scope has no parent in %s on line %d

Fatal error: Uncaught Error: Cannot use "parent" when current class scope has no parent in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
