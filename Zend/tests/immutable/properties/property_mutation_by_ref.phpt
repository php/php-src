--TEST--
Test that declared properties of immutable classes can't be mutated by reference
--XFAIL--
Not yet implemented.
--FILE--
<?php

immutable class Foo
{
    public int $property1;
    public $property2 = 2;
    public int $property3 = 3;
    public int $property4 = 4;

    public function __construct()
    {
        $this->property1 = 1;
    }
}

$foo = new Foo();

foreach ($foo as &$property) {
    try {
        $property = 5;
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }
}

var_dump($foo);

?>
--EXPECT--
object(Foo)#1 (4) {
  ["property1"]=>
  int(1)
  ["property2"]=>
  int(2)
  ["property3"]=>
  int(3)
  ["property4"]=>
  &int(4)
}
