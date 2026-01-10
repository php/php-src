--TEST--
Final class constant in parent may not be overridden by child through trait
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
