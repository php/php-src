--TEST--
Cannot override a final trait constant in a class derived from the class that uses the trait
--FILE--
<?php

trait TestTrait {
    public final const Constant = 123;
}

class ComposingClass {
    use TestTrait;
    public final const Constant = 123;
}

class DerivedClass extends ComposingClass {
    public final const Constant = 456;
}

?>
--EXPECTF--
Fatal error: DerivedClass::Constant cannot override final constant ComposingClass::Constant in %s on line %d
