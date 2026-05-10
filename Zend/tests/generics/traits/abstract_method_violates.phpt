--TEST--
Traits: an abstract trait method using T rejects an implementer with the wrong substituted type
--FILE--
<?php
trait Thing<T> {
    public abstract function foo(T $v): T;
}

class A {
    use Thing<string>;
    public function foo(int $v): int { return $v; }
}
?>
--EXPECTF--
Fatal error: Declaration of A::foo(int $v): int must be compatible with Thing::foo(string $v): string in %s on line %d
