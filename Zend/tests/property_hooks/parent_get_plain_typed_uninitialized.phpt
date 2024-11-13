--TEST--
Using parent::$prop::get() on plain uninitialized typed property
--FILE--
<?php

class P {
    public int $prop;
}

class C extends P {
    public int $prop {
        get => parent::$prop::get();
    }
}

$c = new C();
try {
    var_dump($c->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Typed property C::$prop must not be accessed before initialization
