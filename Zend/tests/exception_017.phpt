--TEST--
Exceptions on improper usage of $this
--FILE--
<?php
abstract class C {
	abstract static function foo();
}

function foo(callable $x) {
}

try {
	C::foo();
} catch (Error $e) {
    echo $e, "\n\n";
}

try {
	foo("C::foo");
} catch (Error $e) {
    echo $e, "\n\n";
}

C::foo();
?>
--EXPECTF--
Error: Cannot call abstract method C::foo() in %s:%d
Stack trace:
#0 {main}

TypeError: Argument 1 passed to foo() must be callable, string given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): foo('C::foo')
#1 {main}


Fatal error: Uncaught Error: Cannot call abstract method C::foo() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
