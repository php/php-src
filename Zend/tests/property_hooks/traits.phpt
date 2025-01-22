--TEST--
Hooked properties in traits
--FILE--
<?php

trait T {
    public $prop {
        get { echo __METHOD__, "\n"; }
        set { echo __METHOD__, "\n"; }
    }
}

class C {
    use T;
}

$test = new C;
$test->prop;
$test->prop = 1;

?>
--EXPECT--
T::$prop::get
T::$prop::set
