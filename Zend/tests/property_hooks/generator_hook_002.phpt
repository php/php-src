--TEST--
Allow calling parent get in generator property hooks
--FILE--
<?php

class A {
    public $prop = 42;
}

class B extends A {
    public $prop {
        get {
            yield parent::$prop::get() + 1;
            yield parent::$prop::get() + 2;
            yield parent::$prop::get() + 3;
        }
    }
}

$b = new B();
foreach ($b->prop as $value) {
    var_dump($value);
}

?>
--EXPECT--
int(43)
int(44)
int(45)
