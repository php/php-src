--TEST--
Test that final properties can't be accessed by reference
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

try {
    foreach ($foo as &$property) {}
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot acquire reference to final property Foo::$property1
