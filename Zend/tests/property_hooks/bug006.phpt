--TEST--
Abstract properties correctly track virtualness
--FILE--
<?php

abstract class Y {
    abstract public string $prop {
        get;
        set => "foo";
    }
}

class X extends Y {
    public string $prop {
        get => "bar";
    }
}

$x = new X;
$x->prop = 1;
var_dump($x->prop);

?>
--EXPECT--
string(3) "bar"
