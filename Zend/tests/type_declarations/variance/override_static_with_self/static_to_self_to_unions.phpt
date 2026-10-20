--TEST--
Overriding static return types with self in final class with union types
--FILE--
<?php

interface A
{
    public function methodScalar(): static|string;
    public function methodIterable1(): static|iterable;
    public function methodIterable2(): static|array;
    public function methodObject1(): static|A;
    public function methodObject2(): static|B;
    public function methodObject3(): static|C;
    public function methodObject4(): static|self;
    public function methodNullable1(): ?static;
    public function methodNullable2(): static|null;
}

final class B implements A
{
    public function methodScalar(): self { return $this; }
    public function methodIterable1(): self|iterable { return $this; }
    public function methodIterable2(): array { return []; }
    public function methodObject1(): self { return $this; }
    public function methodObject2(): B { return $this; }
    public function methodObject3(): C { return new C(); }
    public function methodObject4(): self { return $this; }
    public function methodNullable1(): ?static { return $this; }
    public function methodNullable2(): ?static { return null; }
}

class C
{
}

$b = new B();
var_dump($b->methodScalar());
var_dump($b->methodIterable1());
var_dump($b->methodIterable2());
var_dump($b->methodObject1());
var_dump($b->methodObject2());
var_dump($b->methodObject3());
var_dump($b->methodObject4());
var_dump($b->methodNullable1());
var_dump($b->methodNullable2());
?>
--EXPECTF--
object(B)#1 (0) {
}
object(B)#1 (0) {
}
array(0) {
}
object(B)#1 (0) {
}
object(B)#1 (0) {
}
object(C)#2 (0) {
}
object(B)#1 (0) {
}
object(B)#1 (0) {
}
NULL
