--TEST--
Traits: an abstract trait method using T accepts an implementer matching the substituted type
--FILE--
<?php
trait Thing<T> {
    public abstract function foo(T $v): T;
}

class A {
    use Thing<string>;
    public function foo(string $v): string { return $v; }
}

echo (new A)->foo("hi"), "\n";
?>
--EXPECT--
hi
