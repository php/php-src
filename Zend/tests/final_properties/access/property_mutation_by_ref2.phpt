--TEST--
Test that declared final properties can't be mutated by reference
--XFAIL--
Doesn't work yet
--FILE--
<?php

class Foo
{
    final public int $property1;
    final public int $property2 = 2;

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
object(Foo)#1 (2) {
  ["property1"]=>
  int(1)
  ["property2"]=>
  int(2)
}
