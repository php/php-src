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

// This should error, but doesn't
const BAR = self::class;
var_dump(BAR);

?>
--EXPECT--
string(4) "Test"
string(0) ""
