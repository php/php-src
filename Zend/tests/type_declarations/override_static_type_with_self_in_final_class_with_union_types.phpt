--TEST--
Overriding static return types with self in final class with union types
--FILE--
<?php

interface A
{
    public function methodScalar1(): static|bool;
    public function methodScalar2(): static|bool;
    public function methodScalar3(): static|false;
    public function methodScalar4(): static|true;
    public function methodScalar5(): static|bool;
    public function methodScalar6(): static|bool;
    public function methodScalar7(): static|string;
    public function methodScalar8(): static|string;
    public function methodScalar9(): static|string;
    public function methodScalar10(): static|int;
    public function methodScalar11(): static|int;
    public function methodScalar12(): static|int;
    public function methodScalar13(): static|float;
    public function methodScalar14(): static|float;
    public function methodScalar15(): static|float;
    public function methodIterable1(): static|iterable;
    public function methodIterable2(): static|iterable;
    public function methodIterable3(): static|array;
    public function methodObject1(): static|A;
    public function methodObject2(): static|B;
    public function methodObject3(): static|C;
    public function methodObject4(): static|self;
    public function methodObject5(): static|self;
    public function methodNullable1(): ?static;
    public function methodNullable2(): ?static;
    public function methodNullable3(): static|null;
    public function methodNullable4(): static|null;
    public function methodNullable5(): null;
}

final class B implements A
{
    public function methodScalar1(): self|false { return $this; }
    public function methodScalar2(): self|true { return $this; }
    public function methodScalar3(): false { return false; }
    public function methodScalar4(): true { return true; }
    public function methodScalar5(): bool { return false; }
    public function methodScalar6(): self { return $this; }
    public function methodScalar7(): self|string { return $this; }
    public function methodScalar8(): string { return ''; }
    public function methodScalar9(): self { return $this; }
    public function methodScalar10(): self|int { return $this; }
    public function methodScalar11(): int { return 0; }
    public function methodScalar12(): self { return $this; }
    public function methodScalar13(): self|float { return $this; }
    public function methodScalar14(): float { return 0.0; }
    public function methodScalar15(): self { return $this; }
    public function methodIterable1(): self|iterable { return $this; }
    public function methodIterable2(): self|array { return $this; }
    public function methodIterable3(): array { return []; }
    public function methodObject1(): self { return $this; }
    public function methodObject2(): B { return $this; }
    public function methodObject3(): C { return new C(); }
    public function methodObject4(): self { return $this; }
    public function methodObject5(): B { return $this; }
    public function methodNullable1(): ?static { return $this; }
    public function methodNullable2(): ?static { return null; }
    public function methodNullable3(): static|null { return null; }
    public function methodNullable4(): static|null { return $this; }
    public function methodNullable5(): null { return null; }
}

class C
{
}

$b = new B();
var_dump($b->methodScalar1());
var_dump($b->methodScalar2());
var_dump($b->methodScalar3());
var_dump($b->methodScalar4());
var_dump($b->methodScalar5());
var_dump($b->methodScalar6());
var_dump($b->methodScalar7());
var_dump($b->methodScalar8());
var_dump($b->methodScalar9());
var_dump($b->methodScalar10());
var_dump($b->methodScalar11());
var_dump($b->methodScalar12());
var_dump($b->methodScalar13());
var_dump($b->methodScalar14());
var_dump($b->methodScalar15());
var_dump($b->methodIterable1());
var_dump($b->methodIterable2());
var_dump($b->methodIterable3());
var_dump($b->methodObject1());
var_dump($b->methodObject2());
var_dump($b->methodObject3());
var_dump($b->methodObject4());
var_dump($b->methodObject5());
var_dump($b->methodNullable1());
var_dump($b->methodNullable2());
var_dump($b->methodNullable3());
var_dump($b->methodNullable4());
var_dump($b->methodNullable5());
?>
--EXPECTF--
object(B)#1 (0) {
}
object(B)#1 (0) {
}
bool(false)
bool(true)
bool(false)
object(B)#1 (0) {
}
object(B)#1 (0) {
}
string(0) ""
object(B)#1 (0) {
}
object(B)#1 (0) {
}
int(0)
object(B)#1 (0) {
}
object(B)#1 (0) {
}
float(0)
object(B)#1 (0) {
}
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
object(B)#1 (0) {
}
NULL
NULL
object(B)#1 (0) {
}
NULL
