--TEST--
Cannot use a trait which references parent as a type in a class with no parent, nullable type
--FILE--
<?php
trait TraitExample {
    public function bar(): ?parent { return parent::class; }
}

class A {
    use TraitExample;
}
?>
--EXPECTF--
Fatal error: Cannot use trait which has "parent" as a type when current class scope has no parent in %s on line %d
