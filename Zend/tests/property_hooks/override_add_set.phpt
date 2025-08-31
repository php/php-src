--TEST--
Overridden hooked property can add set to get only property
--FILE--
<?php

class A {
    public $prop {
        get { echo __CLASS__ . '::' . __METHOD__, "\n"; return 42; }
    }
}

class B extends A {
    public $prop {
        get { echo __CLASS__ . '::' . __METHOD__, "\n"; return 42; }
        set { echo __CLASS__ . '::' . __METHOD__, "\n"; }
    }
}

$a = new A;
try {
    $a->prop = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($a->prop);

$b = new B;
$b->prop = 1;
var_dump($b->prop);

?>
--EXPECT--
Property A::$prop is read-only
A::A::$prop::get
int(42)
B::B::$prop::set
B::B::$prop::get
int(42)
