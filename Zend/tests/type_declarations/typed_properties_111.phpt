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
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Cannot assign true to property Foo::$value of type false
