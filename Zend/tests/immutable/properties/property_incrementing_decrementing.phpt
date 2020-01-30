--TEST--
Test that properties of immutable classes can't be incremented/decremented
--FILE--
<?php

immutable class Foo
{
    public $property1;
    public $property2 = 2;
    public int $property3;
    public int $property4 = 4;

    public function __construct()
    {
        //$this->property1 = 1;
        $this->property3 = 3;
    }
}

$foo = new Foo();

try {
    ++$foo->property1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ++$foo->property2;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ++$foo->property3;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ++$foo->property4;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property1++;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property2++;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property3++;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property4++;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    --$foo->property1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    --$foo->property2;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    --$foo->property3;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    --$foo->property4;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property1--;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property2--;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property3--;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property4--;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot increment property Foo::$property3 of an immutable class
Cannot increment property Foo::$property4 of an immutable class
Cannot increment property Foo::$property3 of an immutable class
Cannot increment property Foo::$property4 of an immutable class
Cannot decrement property Foo::$property3 of an immutable class
Cannot decrement property Foo::$property4 of an immutable class
Cannot decrement property Foo::$property3 of an immutable class
Cannot decrement property Foo::$property4 of an immutable class
