--TEST--
Cannot use a trait which references parent as a type in a class with no parent, non-simple union type
--FILE--
<?php

class T {}

trait TraitExample {
    public function bar(): T|parent { return parent::class; }
}

class A {
    use TraitExample;
}
?>
DONE
--EXPECTF--
Fatal error: Cannot use trait which has "parent" as a type when current class scope has no parent in %s on line %d
