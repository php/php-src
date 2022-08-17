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

trait TestTrait2 {
    public final const Constant = 123;
}

class BaseClass2 {
    public final const Constant = 456;
}

class DerivedClass2 extends BaseClass2 {
    use TestTrait2;
}

?>
--EXPECTF--
Fatal error: BaseClass2 and TestTrait2 define the same constant (Constant) in the composition of DerivedClass2. However, the definition differs and is considered incompatible. Class was composed in %s on line %d
