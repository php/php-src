--TEST--
Strict scalar types test
--FILE--
<?php

declare(strict_types=1);

function foo(float|string|true $foo) {
        var_dump($foo);
}

foo(1.0);
foo(1);
foo("1");
foo(true);
foo(false);

?>
--EXPECTF--
float(1)
float(1)
string(1) "1"
bool(true)

Fatal error: Uncaught TypeError: Argument 1 passed to foo() must be true, float or string, false given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): foo(false)
#1 {main}
  thrown in %s on line %d
