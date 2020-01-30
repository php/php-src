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
        throw new Exception("offsetSet() is not supported!");
    }

    function offsetUnset($offset){
        unset($this->$offset);
    }
}

$foo = new Foo();

try {
    $foo["property1"] = "foo";
} catch (Exception $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo["property2"] = "foo";
} catch (Exception $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo["property3"] = "foo";
} catch (Exception $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo["property4"] = "foo";
} catch (Exception $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo["property5"] = "foo";
} catch (Exception $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
offsetSet() is not supported!
offsetSet() is not supported!
offsetSet() is not supported!
offsetSet() is not supported!
offsetSet() is not supported!
