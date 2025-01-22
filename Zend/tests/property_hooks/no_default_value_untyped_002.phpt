--TEST--
Hooked properties with no default value are initialized to null
--FILE--
<?php

class A {
    public $prop;
}

class B extends A {
    public $prop {
        set {}
    }
}

$b = new B;
var_dump($b->prop);

?>
--EXPECT--
NULL
