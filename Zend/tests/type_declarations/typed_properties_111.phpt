--TEST--
Test typed properties allow false
--FILE--
<?php
class Foo {
    public false $value;
}

$foo = new Foo();
$foo->value = false;

try {
    $foo->value = true;
} catch (\TypeError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Cannot assign bool to property Foo::$value of type false
