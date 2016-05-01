--TEST--
Scalar type conversion: int|float|true
--FILE--
<?php

function foo(int|float|true $foo = true) {
        var_dump($foo);
}

foo();
foo(1);
foo(1.1);
foo(false);
foo(true);
foo("1numeric");
foo("1.1numeric");
foo("invalid");
foo("");

?>
--EXPECTF--
bool(true)
int(1)
float(1.1)
int(0)
bool(true)
int(1)
float(1.1)
bool(true)

Fatal error: Uncaught TypeError: Argument 1 passed to foo() must be true, integer or float, string given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): foo('')
#1 {main}
  thrown in %s on line %d

