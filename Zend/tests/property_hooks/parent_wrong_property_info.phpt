--TEST--
Allow calling parent set in property hooks
--FILE--
<?php

class A {
    private int $prop;
}

class B extends A {
    public int $prop {
        get => parent::$prop::get();
    }
}

$b = new B;
try {
    var_dump($b->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot access private property A::$prop
