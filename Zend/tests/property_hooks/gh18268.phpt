--TEST--
GH-18268: array_walk() on object with added property hooks
--FILE--
<?php

class A {
    public $prop = 42;
}

class B extends A {
    public $prop = 42 {
        set {}
    }
}

$b = new B;
array_walk($b, function (&$item) {
    var_dump($item);
});

?>
--EXPECT--
int(42)
