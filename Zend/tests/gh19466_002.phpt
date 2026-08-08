--TEST--
Trait property attribute compatibility validation
--FILE--
<?php

#[Attribute]
class A1 {}

trait T1 {
    #[A1]
    public $prop1;
}

trait T2 {
    public $prop1;
}

class C {
    use T1, T2;
}

?>
--EXPECTF--
Fatal error: T1 and T2 define the same property ($prop1) in the composition of C. However, the definition differs and is considered incompatible. Class was composed in %s on line %d
