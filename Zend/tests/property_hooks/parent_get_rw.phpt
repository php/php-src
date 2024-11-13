--TEST--
RW on parent get is forbidden
--FILE--
<?php

class A {
    public int $prop {
        get {
            return 41;
        }
    }
}

class B extends A {
    public int $prop {
        get {
            return ++parent::$prop::get();
        }
    }
}

$b = new B;
var_dump($b->prop);

?>
--EXPECTF--
Fatal error: Can't use method return value in write context in %s on line %d
