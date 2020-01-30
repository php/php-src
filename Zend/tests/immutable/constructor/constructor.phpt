--TEST--
Test the constructor of immutable classes
--FILE--
<?php

// Instantiating an immutable class without a constructor

immutable class Foo
{
}

$foo = new Foo();

// Instantiating an immutable class with a constructor

immutable class Bar
{
    public function __construct()
    {
    }
}

// Instantiating an immutable class that calls parent::__construct() during instantiation

$bar = new Bar();

immutable class Baz extends Bar
{
    public function __construct()
    {
        parent::__construct();
        parent::__construct();
    }
}

$baz = new Bar();

?>
--EXPECT--
