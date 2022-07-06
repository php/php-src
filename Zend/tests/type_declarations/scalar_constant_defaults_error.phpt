--TEST--
Scalar type - default via constants - error condition
--FILE--
<?php

const STRING_VAL = "test";

function int_val(int $a = STRING_VAL): int {
    return $a;
}

var_dump(int_val());

?>
--EXPECTF--
Fatal error: Uncaught TypeError: int_val(): Argument #1 ($a) must be of type int, string given, called in %s:%d
Stack trace:
#0 %s(%d): int_val()
#1 {main}
  thrown in %s on line %d
