--TEST--
Repeated assign of a variable to mismatched property type must not succeed
--FILE--
<?php

class A {
        public int $foo;
}

class B {
        public A $foo;
}

$objs = [new A, new A];
$v = 1;

foreach ($objs as $obj) {
        $obj->foo = $v;
        $v = new A;
        $obj = new B;
        $obj->foo = $v;
}

var_dump($objs);

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property A::$foo must be int, A used in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
