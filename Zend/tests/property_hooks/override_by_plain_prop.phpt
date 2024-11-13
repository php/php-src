--TEST--
Hooks are inherited to plain property
--FILE--
<?php

class A {
    public $prop {
        get { echo __METHOD__, "\n"; return 42; }
        set { echo __METHOD__, "\n"; }
    }
}

class B extends A {
    public $prop;
}

$a = new A();
$a->prop = 43;
var_dump($a->prop);

$b = new B();
$b->prop = 43;
var_dump($b->prop);

?>
--EXPECT--
A::$prop::set
A::$prop::get
int(42)
A::$prop::set
A::$prop::get
int(42)
