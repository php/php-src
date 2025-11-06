--TEST--
GH-20270: Parent hook call with named arguments
--CREDITS--
Viet Hoang Luu (@vi3tL0u1s)
--FILE--
<?php

class A {
    public mixed $prop1;
    public mixed $prop2 {
        set(mixed $custom) => $custom;
    }
}

class B extends A {
    public mixed $prop1 {
        set {
            parent::$prop1::set(value: 42);
            parent::$prop1::set(unknown: 43);
        }
    }
    public mixed $prop2 {
        set {
            parent::$prop2::set(custom: 42);
            parent::$prop2::set(value: 43);
        }
    }
}

$b = new B();

try {
    $b->prop1 = 0;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($b->prop1);

try {
    $b->prop2 = 0;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($b->prop2);

?>
--EXPECT--
Unknown named parameter $unknown
int(42)
Unknown named parameter $value
int(42)
