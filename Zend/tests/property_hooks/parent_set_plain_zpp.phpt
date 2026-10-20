--TEST--
parent::$prop::set() ZPP
--FILE--
<?php

class A {
    public int $prop;
}

class B extends A {
    public int $prop {
        set {
            parent::$prop::set($value, 42);
        }
    }
}

$b = new B();
try {
    $b->prop = 42;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ArgumentCountError: A::$prop::set() expects exactly 1 argument, 2 given
