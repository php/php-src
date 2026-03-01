--TEST--
Trait property attribute compatibility validation
--FILE--
<?php

#[Attribute]
class A1 {}

trait T1 {
    #[A1(1, 2)]
    public $prop1;
}

trait T2 {
    #[A1(1, 2)]
    public $prop1;
}

class C {
    use T1, T2;
}

?>
===DONE===
--EXPECT--
===DONE===
