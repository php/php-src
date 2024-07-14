--TEST--
Using parent::$prop::get() on plain property
--FILE--
<?php

class P {
    public $prop = 42;
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
int(42)
