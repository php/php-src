--TEST--
Using parent::$prop::set() on plain property
--FILE--
<?php

class P {
    public $prop;
}

class C extends P {
    public $prop {
        set {
            var_dump(parent::$prop::set($value));
        }
    }
}

$c = new C();
$c->prop = 42;

?>
--EXPECT--
int(42)
