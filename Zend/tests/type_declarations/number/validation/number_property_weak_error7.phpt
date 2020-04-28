--TEST--
Test that the number property type doesn't accept any non-numeric value in weak mode
--FILE--
<?php

class Foo
{
    public number $property1;

    public function __construct()
    {
        $this->property1 = fopen(__FILE__, "r");
    }
}

$foo = new Foo();

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot assign resource to property Foo::$property1 of type number in %s:%d
Stack trace:
#0 %s(%d): Foo->__construct()
#1 {main}
  thrown in %s on line %d
