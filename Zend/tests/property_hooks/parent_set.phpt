--TEST--
Allow calling parent set in property hooks
--FILE--
<?php

class A {
    public int $prop {
        set {
            var_dump($value);
        }
    }
}

class B extends A {
    public int $prop {
        set {
            parent::$prop::set($value + 1);
        }
    }
}

$a = new A;
$a->prop = 41;

$b = new B;
$b->prop = 41;

?>
--EXPECT--
int(41)
int(42)
