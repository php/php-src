--TEST--
Test that the constructor of immutable classes can't be invoked after instantiation
--FILE--
<?php

// Explicitly calling __construct() that is not defined

immutable class Foo
{
}

$foo = new Foo();
try {
    $foo->__construct();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

// Explicitly calling __construct() that is defined

immutable class Bar
{
    public function __construct()
    {
    }
}

$bar = new Bar();
try {
    $bar->__construct();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Call to undefined method Foo::__construct()
Cannot call the constructor of an immutable class Bar
