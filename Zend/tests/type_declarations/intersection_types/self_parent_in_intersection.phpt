--TEST--
Using self/parent/static part of an intersection
--FILE--
<?php

interface X {}
interface Y {}
interface Z {}

class A {
    public function test(): self&X {
        return $this;
    }
}
class B extends A implements X {}
class BError extends A {}

class C extends B {
    public function parentIntersection(): parent&X {
        return new B();
    }
}
class D extends B {
    public function parentIntersection(): parent&X {
        return new A();
    }
}

$b = new B();
$be = new BError();
$c = new C();
$d = new D();

// Self tests
var_dump($b->test());
try {
    var_dump($be->test());
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

// Parent tests
var_dump($c->parentIntersection());
try {
    var_dump($d->parentIntersection());
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
object(B)#1 (0) {
}
A::test(): Return value must be of type A&X, BError returned
object(B)#6 (0) {
}
D::parentIntersection(): Return value must be of type B&X, A returned
