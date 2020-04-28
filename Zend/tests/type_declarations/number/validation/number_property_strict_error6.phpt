--TEST--
Test that the number property type doesn't accept any non-numeric type in strict mode
--FILE--
<?php
declare(strict_types=1);

class Foo
{
    public number $property1;

    public function __construct()
    {
        $this->property1 = new stdClass();
    }
}

$foo = new Foo();

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot assign stdClass to property Foo::$property1 of type number in %s:%d
Stack trace:
#0 %s(%d): Foo->__construct()
#1 {main}
  thrown in %s on line %d
