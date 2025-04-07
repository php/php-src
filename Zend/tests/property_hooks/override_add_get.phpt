--TEST--
Overridden hooked property can add get to set only property
--FILE--
<?php

class A {
    public $prop {
        set { echo __CLASS__ . '::' . __METHOD__, "\n"; }
    }
}

class B extends A {
    public $prop {
        get { echo __CLASS__ . '::' . __METHOD__, "\n"; return 42; }
        set { echo __CLASS__ . '::' . __METHOD__, "\n"; }
    }
}

$a = new A;
$a->prop = 1;
try {
    var_dump($a->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$b = new B;
$b->prop = 1;
var_dump($b->prop);

?>
--EXPECT--
A::A::$prop::set
Property A::$prop is write-only
B::B::$prop::set
B::B::$prop::get
int(42)
