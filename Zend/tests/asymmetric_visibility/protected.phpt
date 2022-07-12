--TEST--
Asymmetric visibility protected(set)
--FILE--
<?php

class Foo {
    public protected(set) string $bar = 'bar';

    protected(set) string $baz = 'baz';

    public function setBarPrivate($bar) {
        $this->bar = $bar;
    }

    public function setBazPrivate($baz) {
        $this->baz = $baz;
    }
}

class FooChild extends Foo {
    public function setBarProtected($bar) {
        $this->bar = $bar;
    }

    public function setBazProtected($baz) {
        $this->baz = $baz;
    }
}

$foo = new FooChild();
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

try {
    $foo->baz = 'baz';
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$foo->setbazPrivate('baz2');
var_dump($foo->baz);

$foo->setbazProtected('baz3');
var_dump($foo->baz);


?>
--EXPECT--
string(3) "bar"
Cannot modify protected(set) property Foo::$bar from global scope
string(3) "baz"
string(3) "qux"
Cannot modify protected(set) property Foo::$baz from global scope
string(4) "baz2"
string(4) "baz3"
