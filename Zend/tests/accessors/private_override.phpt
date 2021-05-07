--TEST--
Overriding private accessors
--FILE--
<?php

class A {
    private $prop1;
    private $prop2 {
        get { echo __METHOD__, "\n"; }
        set { echo __METHOD__, "\n"; }
    }
    public $prop3 {
        get { echo __METHOD__, "\n"; }
        private set { echo __METHOD__, "\n"; }
    }
    public $prop4 {
        private get { echo __METHOD__, "\n"; }
        set { echo __METHOD__, "\n"; }
    }

    public function testPrivate() {
        $this->prop1;
        $this->prop1 = 1;
        $this->prop2;
        $this->prop2 = 1;
        $this->prop3;
        $this->prop3 = 1;
        $this->prop4;
        $this->prop4 = 1;
    }
}

class B extends A {
    public $prop1 {
        get { echo __METHOD__, "\n"; }
        set { echo __METHOD__, "\n"; }
    }
    public $prop2 {
        get { echo __METHOD__, "\n"; }
        set { echo __METHOD__, "\n"; }
    }
    public $prop3 {
        get { echo __METHOD__, "\n"; }
        set { echo __METHOD__, "\n"; }
    }
    public $prop4 {
        get { echo __METHOD__, "\n"; }
        set { echo __METHOD__, "\n"; }
    }
}

$a = new A;
$a->testPrivate();
echo "\n";

$b = new B;
$b->testPrivate();
echo "\n";

$b->prop1;
$b->prop1 = 1;
$b->prop2;
$b->prop2 = 1;
$b->prop3;
$b->prop3 = 1;
$b->prop4;
$b->prop4 = 1;

?>
--EXPECT--
A::$prop2::get
A::$prop2::set
A::$prop3::get
A::$prop3::set
A::$prop4::get
A::$prop4::set

A::$prop2::get
A::$prop2::set
B::$prop3::get
A::$prop3::set
A::$prop4::get
B::$prop4::set

B::$prop1::get
B::$prop1::set
B::$prop2::get
B::$prop2::set
B::$prop3::get
B::$prop3::set
B::$prop4::get
B::$prop4::set
