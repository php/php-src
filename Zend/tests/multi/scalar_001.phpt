--TEST--
Scalar type conversion: int|float|string
--FILE--
<?php

function foo(int|float|string $foo) {
	var_dump($foo);
}

foo(1);
foo(1.0);
foo(1.1);
foo("abc");
foo(true);
foo(false);
foo(new class { function __toString() { return "1"; } });
foo(null);

?>
--EXPECTF--
int(1)
float(1)
float(1.1)
string(3) "abc"
int(1)
int(0)
string(1) "1"

Fatal error: Uncaught TypeError: Argument 1 passed to foo() must be integer, float or string, null given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): foo(NULL)
#1 {main}
  thrown in %s on line %d

