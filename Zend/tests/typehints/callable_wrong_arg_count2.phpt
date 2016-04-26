--TEST--
Passing a function that expects 2 arguments where it is expected to take only one should throw a type error
Even if the second argument is optional
--FILE--
<?php

function foo(callable($a) $cb) {

}

function boo($a, $b = null) {}

foo('boo');

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to foo() must be compliant with callable($a), incompatible callable($a, $b) given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): foo('boo')
#1 {main}
  thrown in %s on line %d

