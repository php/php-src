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
OK
--EXPECT--
OK
