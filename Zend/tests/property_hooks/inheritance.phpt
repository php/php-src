--TEST--
Basic property hook inheritance
--FILE--
<?php

class A {
    public $prop {
        get { return "A"; }
        set { echo __METHOD__, "\n"; }
    }
}

class B extends A {
    public $prop {
        get { return "B"; }
    }
}

$a = new A;
var_dump($a->prop);
$a->prop = 1;

$b = new B;
var_dump($b->prop);
$b->prop = 1;

?>
--EXPECT--
string(1) "A"
A::$prop::set
string(1) "B"
A::$prop::set
