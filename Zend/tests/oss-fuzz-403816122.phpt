--TEST--
OSS-Fuzz #403816122: Segfault when initializing default properties of child prop with added hooks
--FILE--
<?php

const X = 'x';

class P {
    public $prop;
}

class C extends P {
    public $prop = X {
        get => 'y';
    }
}

var_dump((new C)->prop);

?>
--EXPECT--
string(1) "y"
