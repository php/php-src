--TEST--
Test that final properties can't be incremented/decremented by reference
--FILE--
<?php

class Foo
{
    final public int $property1;
    final public int $property2 = 2;

    public function __construct()
    {
        $this->property1 = 1;
    }
}

$foo = new Foo();
foreach ($foo as &$property) {
    try {
        ++$property;
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }
}

var_dump($foo->property1);
var_dump($foo->property2);

$foo = new Foo();
foreach ($foo as &$property) {
    try {
        $property++;
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }
}

var_dump($foo->property1);
var_dump($foo->property2);

foreach ($foo as &$property) {
    try {
        --$property;
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }
}

var_dump($foo->property1);
var_dump($foo->property2);

foreach ($foo as &$property) {
    try {
        $property--;
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }
}

var_dump($foo->property1);
var_dump($foo->property2);

?>
--EXPECT--
Cannot increment a reference held by final property Foo::$property1
Cannot increment a reference held by final property Foo::$property2
int(1)
int(2)
Cannot increment a reference held by final property Foo::$property1
Cannot increment a reference held by final property Foo::$property2
int(1)
int(2)
Cannot decrement a reference held by final property Foo::$property1
Cannot decrement a reference held by final property Foo::$property2
int(1)
int(2)
Cannot decrement a reference held by final property Foo::$property1
Cannot decrement a reference held by final property Foo::$property2
int(1)
int(2)
