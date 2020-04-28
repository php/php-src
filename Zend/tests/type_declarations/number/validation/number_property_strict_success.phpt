--TEST--
Test that the number property type accepts integer and float types in strict mode
--FILE--
<?php
declare(strict_types=1);

class Foo
{
    public number $property1 = 1;
    public number $property2 = 3.14;
}

$foo = new Foo();

?>
--EXPECT--
