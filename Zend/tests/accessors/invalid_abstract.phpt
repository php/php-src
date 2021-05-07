--TEST--
Implementing abstract accessors
--FILE--
<?php

abstract class A {
    public $prop1 {
        abstract get;
        set { echo __METHOD__, "\n"; }
    }
    public $prop2 {
        get { echo __METHOD__, "\n"; }
        abstract set;
    }
    public $prop3 { abstract get; abstract set; }
}

class B extends A {
    public $prop1 {
        get { echo __METHOD__, "\n"; }
    }
    public $prop2 {
        set { echo __METHOD__, "\n"; }
    }
    public $prop3;
}

$b = new B;
$b->prop1;
$b->prop1 = 1;
$b->prop2;
$b->prop2 = 1;
$b->prop3;
$b->prop3 = 1;

?>
--EXPECT--
B::$prop1::get
A::$prop1::set
A::$prop2::get
B::$prop2::set
