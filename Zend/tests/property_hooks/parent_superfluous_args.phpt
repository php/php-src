--TEST--
Extra args to parent hook call
--FILE--
<?php

class A {
    public $backed;
    public $virtual { get {} set {} }
}

class B extends A {
    public $backed {
        get {
            return parent::$backed::get(42);
        }
        set {
            parent::$backed::set($value, 42);
        }
    }
    public $virtual {
        get {
            return parent::$virtual::get(42);
        }
        set {
            parent::$virtual::set($value, 42);
        }
    }
}

$b = new B();
// Calling a parent hook with extra args is ok, because they are user functions.
var_dump($b->virtual = 42);
var_dump($b->virtual);
// Calling the implicit parent hook with extra args is not ok, since it is an internal function.
try {
    var_dump($b->backed = 42);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($b->backed);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
int(42)
NULL
A::$backed::set() expects exactly 1 argument, 2 given
A::$backed::get() expects exactly 0 arguments, 1 given
