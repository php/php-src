--TEST--
Using parent::$prop::get() on plain untyped uninitialized property
--FILE--
<?php

class P {
    public $prop;
}

class C extends P {
    public $prop {
        get => parent::$prop::get();
    }
}

$c = new C();
var_dump($c->prop);

?>
--EXPECT--
NULL
