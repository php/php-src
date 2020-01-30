--TEST--
Test that ArrayAccess works for immutable classes as expected
--FILE--
<?php

immutable class Foo implements ArrayAccess
{
    private $property1;
    private $property2 = "";
    private string $property3;
    private string $property4 = "";

    function offsetExists($offset)
    {
        return isset($this->$offset);
    }

    function offsetGet($offset)
    {
        return $this->$offset ?? null;
    }

    function offsetSet($offset, $value)
    {
        $this->$offset = $value;
    }

    function offsetUnset($offset){
        unset($this->$offset);
    }
}

$foo = new Foo();

try {
    $foo["property1"] = "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo["property2"] = "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

$foo["property3"] = "foo";

try {
    $foo["property4"] = "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo["property5"] = "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot assign value to property Foo::$property1 of an immutable class more than once
Cannot assign value to property Foo::$property2 of an immutable class more than once
Cannot assign value to property Foo::$property4 of an immutable class more than once
Cannot declare dynamic property Foo:$property5 of an immutable class
