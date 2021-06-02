--TEST--
Using self/parent/static part of an intersection
--FILE--
<?php

interface X {}
interface Y {}
interface Z {}

abstract class A {
    abstract public function test(): static&X;
}
class B extends A implements X {
     public function test(): static&X {
         return new self();
     }
}

class C extends B {
    public function test(): static&X {
        return new $this;
    }
}
class CError extends B {}

$c = new C();
$ce = new CError();

// Self tests
var_dump($c->test());
try {
    var_dump($ce->test());
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
object(C)#3 (0) {
}
B::test(): Return value must be of type X&CError, B returned
