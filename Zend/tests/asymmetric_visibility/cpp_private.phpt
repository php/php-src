--TEST--
Asymmetric visibility private(set) CPP
--FILE--
<?php

class Foo {
    public function __construct(
        public private(set) string $bar,
    ) {}

    public function setBar($bar) {
        $this->bar = $bar;
    }
}

$foo = new Foo('bar');
var_dump($foo->bar);

try {
    $foo->bar = 'baz';
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$foo->setBar('baz');
var_dump($foo->bar);

?>
--EXPECT--
string(3) "bar"
Cannot modify private(set) property Foo::$bar from global scope
string(3) "baz"
