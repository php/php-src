--TEST--
GH-15240: Trait hook leads to infinite recursion
--FILE--
<?php

trait T {
    public $prop {
        set => $value;
    }
}

class C {
    use T;
}

$c = new C;
$c->prop = 42;
var_dump($c->prop);

?>
--EXPECT--
int(42)
