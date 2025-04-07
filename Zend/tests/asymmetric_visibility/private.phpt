--TEST--
Asymmetric visibility private(set)
--FILE--
<?php

class Foo {
    public private(set) string $bar = 'bar';

    private(set) string $baz = 'baz';

    public function setBar($bar) {
        $this->bar = $bar;
    }

    public function setBaz($baz) {
        $this->baz = $baz;
    }
}

class FooChild extends Foo {
    public function modifyBar($bar) {
        $this->bar = $bar;
    }
}

$foo = new Foo();
var_dump($foo->bar);

try {
    $foo->bar = 'baz';
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$foo->setBar('baz');
var_dump($foo->bar);

try {
    $foo->baz = 'baz2';
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$foo->setBaz('baz2');
var_dump($foo->baz);

$child = new FooChild();
try {
    $child->modifyBar('baz');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(3) "bar"
Cannot modify private(set) property Foo::$bar from global scope
string(3) "baz"
Cannot modify private(set) property Foo::$baz from global scope
string(4) "baz2"
Cannot modify private(set) property Foo::$bar from scope FooChild
