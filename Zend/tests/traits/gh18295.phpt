--TEST--
GH-18295: Parent self is substitutable with child self through trait
--FILE--
<?php

class A {
    public function create(): ?A {}
}

trait T {
    public function create(): self {}
}

class B extends A {
    use T;
}

?>
===DONE===
--EXPECT--
===DONE===
