--TEST--
Valid $this inheritance
--FILE--
<?php

class A {
    public function method1(): object {}
    public function method2(): self {}
    public function method3(): static {}
    public function method4(): $this {}
    public function method5(): int|B {}
}
class B extends A {
    public function method1(): $this {}
    public function method2(): $this {}
    public function method3(): $this {}
    public function method4(): $this {}
    public function method5(): int|$this {}
}

?>
===DONE===
--EXPECT--
===DONE===
