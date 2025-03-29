--TEST--
The same name constant of a trait used in a class that inherits a constant defined in a parent can be defined only if they are compatible.
--FILE--
<?php

trait TestTrait1 {
    public final const Constant = 123;
}

class BaseClass1 {
    public final const Constant = 123;
}

class DerivedClass1 extends BaseClass1 {
    use TestTrait1;
}

?>
--EXPECTF--
Fatal error: DerivedClass1::Constant cannot override final constant BaseClass1::Constant in %s on line %d
