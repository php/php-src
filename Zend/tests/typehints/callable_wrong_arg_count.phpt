--TEST--
Passing a function that expects 2 arguments where it is expected to take only one should throw a type error
--FILE--
<?php

function foo(callable($a) $cb) {

}

function boo($a, $b) {}

foo('boo');

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to foo() must be callable of compliant signature: callable($a), callable($a, $b) given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): foo('boo')
#1 {main}
  thrown in %s on line %d

