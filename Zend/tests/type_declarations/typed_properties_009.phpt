--TEST--
Test typed properties unset leaves properties in an uninitialized state
--FILE--
<?php
class Foo {
    public int $bar;

    public function __get($name) {
        var_dump($name);
        /* return value has to be compatible with int */
        return 0;
    }
}

$foo = new Foo();

unset($foo->bar);

var_dump($foo->bar);
?>
--EXPECT--
string(3) "bar"
int(0)
