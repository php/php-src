--TEST--
Closure should be covariant with callable
--FILE--
<?php

class A {
    public function foo(Closure $c): callable {}
}
class B extends A {
    public function foo(callable $c): Closure {}
}
?>
--EXPECTF--
Fatal error: Declaration of B::foo(callable $c): Closure must be compatible with A::foo(Closure $c): callable in %s on line %d
