--TEST--
Override plain property with hooked property
--FILE--
<?php

class A {
    public $prop;
}

class B extends A {
    public $prop {
        get {
            return 'B::$prop::get()';
        }
        set {
            echo "B::\$prop::set($value)\n";
        }
    }
}

$a = new A();
$a->prop = 42;
echo $a->prop, "\n";

$b = new B();
$b->prop = 42;
echo $b->prop, "\n";

?>
--EXPECT--
42
B::$prop::set(42)
B::$prop::get()
