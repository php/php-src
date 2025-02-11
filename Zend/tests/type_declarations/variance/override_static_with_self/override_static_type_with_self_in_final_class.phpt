--TEST--
Overriding static return types with self in final class
--FILE--
<?php

interface A
{
    public function method1(): static;
}

abstract class B
{
    abstract public function method2(): static;
}

trait C
{
    abstract public function method3(): static;
}

final class Foo extends B implements A
{
    use C;

    public function method1(): self
    {
        return $this;
    }

    public function method2(): self
    {
        return $this;
    }

    public function method3(): self
    {
        return $this;
    }
}

final class Bar extends B implements A
{
    use C;

    public function method1(): Bar
    {
        return $this;
    }

    public function method2(): Bar
    {
        return $this;
    }

    public function method3(): Bar
    {
        return $this;
    }
}

$foo = new Foo();

var_dump($foo->method1());
var_dump($foo->method2());
var_dump($foo->method3());

$bar = new Bar();

var_dump($bar->method1());
var_dump($bar->method2());
var_dump($bar->method3());
?>
--EXPECTF--
object(Foo)#1 (0) {
}
object(Foo)#1 (0) {
}
object(Foo)#1 (0) {
}
object(Bar)#2 (0) {
}
object(Bar)#2 (0) {
}
object(Bar)#2 (0) {
}
