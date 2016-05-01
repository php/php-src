--TEST--
Scalar type conversion: __toString in unions
--FILE--
<?php

function foo(int|string $foo) {
        var_dump($foo);
}

foo(new class { function __toString() { return "1"; } });
foo(new class { });

?>
--EXPECTF--
string(1) "1"

Fatal error: Uncaught TypeError: Argument 1 passed to foo() must be integer or string, instance of class@anonymous given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): foo(Object(class@anonymous))
#1 {main}
  thrown in %s on line %d

