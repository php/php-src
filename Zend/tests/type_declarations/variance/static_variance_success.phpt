--TEST--
Success cases for static variance
--FILE--
<?php

class A {
    public function test1(): self {}
    public function test2(): B {}
    public function test3(): object {}
    public function test4(): X|Y|self {}
    public function test5(): ?static {}
}

class B extends A {
    public function test1(): static {}
    public function test2(): static {}
    public function test3(): static {}
    public function test4(): X|Y|static {}
    public function test5(): static {}
}

?>
===DONE===
--EXPECT--
===DONE===
