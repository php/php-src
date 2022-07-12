--TEST--
Asymmetric visibility __unset
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

    public function __unset($name) {
        echo __METHOD__, "\n";
    }
}

function test($foo) {
    try {
        unset($foo->bar);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

$foo = new Foo();
test($foo);

$foo->unsetBar();
test($foo);

$foo->setBar('bar');
test($foo);

$foo->unsetBar();
test($foo);

?>
--EXPECT--
Cannot unset private(set) property Foo::$bar from global scope
Foo::__unset
Cannot unset private(set) property Foo::$bar from global scope
Foo::__unset
