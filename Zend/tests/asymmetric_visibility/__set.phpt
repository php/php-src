--TEST--
Asymmetric visibility __set
--FILE--
<?php

class Foo {
    public private(set) string $bar;

    public function setBar($bar) {
        $this->bar = $bar;
    }

    public function unsetBar() {
        unset($this->bar);
    }

    public function __set(string $name, mixed $value) {
        echo __CLASS__, '::', __METHOD__, "\n";
    }
}

$foo = new Foo();
try {
    $foo->bar = 'baz';
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$foo->setBar('baz');
try {
    $foo->bar = 'baz';
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$foo->unsetBar();
$foo->bar = 'baz';

?>
--EXPECT--
Cannot modify private(set) property Foo::$bar from global scope
Cannot modify private(set) property Foo::$bar from global scope
Foo::Foo::__set
