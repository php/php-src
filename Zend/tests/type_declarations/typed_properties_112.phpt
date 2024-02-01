--TEST--
Test typed properties allow true
--FILE--
<?php
class Foo {
    public true $value;
}

$foo = new Foo();

try {
    $foo->value = false;
} catch (\TypeError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Cannot assign bool to property Foo::$value of type true
