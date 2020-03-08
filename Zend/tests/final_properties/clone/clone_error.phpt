--TEST--
Test that final properties can't be modified after cloning
--FILE--
<?php

class Foo
{
    final public int $property1;
    final public string $property2 = "";
}

$foo = new Foo();
$foo->property1 = 1;
$bar = clone $foo;

try {
    $bar->property1 = 2;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $bar->property2 = "Baz";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $var = &$foo->property2;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot modify final property Foo::$property1 after initialization
Cannot modify final property Foo::$property2 after initialization
Cannot acquire reference on final property Foo::$property2
