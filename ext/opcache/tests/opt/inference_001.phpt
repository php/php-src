--TEST--
Type inference 001: Invalid type narrowing warning
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function test() {
    for ($i = 0; $i < 2; $i++) {
        $obj->x;
        $obj = new stdClass;
    }
}
test();

class Test {
    public int $x = 1;
}

function test2() {
    for ($i = 0; $i < 2; $i++) {
        $obj->x;
        $obj = new Test;
    }
}
test2();
?>
DONE
--EXPECTF--
Warning: Undefined variable $obj in %s on line %d

Warning: Attempt to read property "x" on null in %s on line %d

Warning: Undefined property: stdClass::$x in %s on line %d

Warning: Undefined variable $obj in %s on line %d

Warning: Attempt to read property "x" on null in %s on line %d
DONE
