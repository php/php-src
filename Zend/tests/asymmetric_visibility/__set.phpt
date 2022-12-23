--TEST--
Asymmetric visibility __set
--FILE--
<?php

class Foo {
    public private(set) string $bar = 'bar';

    public function setBar($bar) {
        $this->bar = $bar;
    }

    public function __set(string $name, mixed $value) {
        echo __CLASS__, '::', __METHOD__, "\n";
    }
}

$foo = new Foo();
var_dump($foo->bar);

$foo->bar = 'baz';
var_dump($foo->bar);

$foo->setBar('baz');
var_dump($foo->bar);

?>
--EXPECT--
string(3) "bar"
Foo::Foo::__set
string(3) "bar"
string(3) "baz"
