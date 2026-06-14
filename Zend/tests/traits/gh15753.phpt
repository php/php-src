--TEST--
GH-15753: Trait can override property declared in parent class of used class
--FILE--
<?php

class P {
    public $prop = 1;
}

trait T {
    public $prop = 2;
}

class C extends P {
    use T;
}

$c = new C();
var_dump($c->prop);

?>
--EXPECT--
int(2)
