--TEST--
Use of self::class inside a constant in an unknown scope
--FILE--
<?php

class Test {
    public function foobar() {
        eval("
            const FOO = self::class;
            var_dump(FOO);
        ");
    }
}
(new Test)->foobar();

const BAR = self::class;
var_dump(BAR);

?>
--EXPECTF--
string(4) "Test"

Fatal error: Uncaught Error: Cannot use "self" when no class scope is active in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
