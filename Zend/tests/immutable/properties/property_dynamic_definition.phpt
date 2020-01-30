--TEST--
Test that properties of immutable classes cannot be declared dynamically
--FILE--
<?php

immutable class Foo
{
}

$foo = new Foo();

try {
    $foo->property1 = "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::$property1 = "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot declare dynamic property Foo:$property1 of an immutable class
Access to undeclared static property: Foo::$property1
