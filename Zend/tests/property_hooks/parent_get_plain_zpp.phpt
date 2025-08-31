--TEST--
parent::$prop::get() ZPP
--FILE--
<?php

class A {
    public int $prop;
}

class B extends A {
    public int $prop {
        get {
            return parent::$prop::get(new stdClass());
        }
    }
}

$b = new B();
try {
    var_dump($b->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
A::$prop::get() expects exactly 0 arguments, 1 given
