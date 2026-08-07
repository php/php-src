--TEST--
Test typed properties allow null
--FILE--
<?php
class Foo {
    public null $value;
}

$foo = new Foo();
$foo->value = null;

try {
    $foo->value = 1;
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Cannot assign int to property Foo::$value of type null
