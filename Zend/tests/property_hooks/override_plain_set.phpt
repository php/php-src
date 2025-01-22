--TEST--
Overridden set of plain property
--FILE--
<?php

class A {
    public $prop;
}

class B extends A {
    public $prop {
        set { echo __CLASS__ . '::' . __METHOD__, "\n"; }
    }
}

$b = new B;
$b->prop = 1;
var_dump($b->prop);

?>
--EXPECT--
B::B::$prop::set
NULL
