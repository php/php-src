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
--EXPECTF--
Deprecated: array_walk(): Passing an object for argument #1 $array to array_walk() is deprecated, call get_object_vars() first instead in %s on line %d
int(42)
