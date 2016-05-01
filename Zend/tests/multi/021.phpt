--TEST--
union test with classes
--FILE--
<?php

class A {}
class B {}
class C {}
class CC extends C {}

function foo(B|C $foo): B|C {
	var_dump($foo);
	return $foo;
}

foo(new CC);
foo(new C);
foo(new B);
foo(new A);

?>
--EXPECTF--
object(CC)#1 (0) {
}
object(C)#1 (0) {
}
object(B)#1 (0) {
}

Fatal error: Uncaught TypeError: Argument 1 passed to foo() must be B or C, instance of A given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): foo(Object(A))
#1 {main}
  thrown in %s on line %d

