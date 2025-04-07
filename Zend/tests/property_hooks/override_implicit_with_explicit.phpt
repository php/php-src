--TEST--
Property can be overridden by hooked property
--FILE--
<?php

class A {
    public $prop;
}

class B extends A {
    public $prop {
        get { echo __CLASS__ . '::' . __METHOD__ . "\n"; return 3; }
        set { echo __CLASS__ . '::' . __METHOD__ . "\n"; }
    }
}

$a = new A;
$a->prop = 1;
var_dump($a->prop);

$b = new B;
$b->prop = 2;
var_dump($b->prop);

?>
--EXPECT--
int(1)
B::B::$prop::set
B::B::$prop::get
int(3)
