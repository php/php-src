--TEST--
Test that the __set() magic method can be invoked
--FILE--
<?php

immutable class Foo
{
    public function __set($name, $value): void
    {
       throw new Exception("__set() is not supported!");
    }
}

$foo = new Foo();

try {
    $foo->property1 = "foo";
} catch (Exception $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
__set() is not supported!
