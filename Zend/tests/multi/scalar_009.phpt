--TEST--
Scalar type conversion: int|float|false
--FILE--
<?php

function foo(int|float|false $foo) {
        var_dump($foo);
}

foo(1);
foo(1.1);
foo(false);
foo(true);
foo("0");
foo("1numeric");
foo("1.1numeric");
foo("");
foo("invalid");

?>
--EXPECTF--
int(1)
float(1.1)
bool(false)
int(1)
int(0)

Notice: A non well formed numeric value encountered in %s on line %d
int(1)

Notice: A non well formed numeric value encountered in %s on line %d
float(1.1)
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to foo() must be false, integer or float, string given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): foo('invalid')
#1 {main}
  thrown in %s on line %d

