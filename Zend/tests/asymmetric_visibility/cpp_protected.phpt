--TEST--
Asymmetric visibility protected(set) CPP
--FILE--
<?php

class Foo {
    public function __construct(
        public protected(set) string $bar,
    ) {}

    public function setBarPrivate($bar) {
        $this->bar = $bar;
    }
}

class FooChild extends Foo {
    public function setBarProtected($bar) {
        $this->bar = $bar;
    }
}

$foo = new FooChild('bar');
var_dump($foo->bar);

try {
    $foo->bar = 'baz';
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$foo->setBarPrivate('baz');
var_dump($foo->bar);

$foo->setBarProtected('qux');
var_dump($foo->bar);

?>
--EXPECT--
string(3) "bar"
Cannot modify protected(set) property Foo::$bar from global scope
string(3) "baz"
string(3) "qux"
