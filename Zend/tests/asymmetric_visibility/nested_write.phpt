--TEST--
Asymmetric visibility nested write
--FILE--
<?php

class Foo {
    public private(set) Bar $bar;

    public function __construct() {
        $this->bar = new Bar;
    }
}

class Bar {
    public int $baz;
}

$foo = new Foo();
$foo->bar->baz = 42;
var_dump($foo->bar->baz);

?>
--EXPECT--
int(42)
