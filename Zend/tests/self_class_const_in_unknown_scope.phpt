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

Fatal error: Uncaught Error: "self" cannot be used in the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
