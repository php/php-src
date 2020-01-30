--TEST--
Test that properties of immutable classes can't be incremented/decremented by reference
--XFAIL--
There is an issue on 32-bit which I haven't figured out yet
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
foreach ($foo as &$property) {
    try {
        ++$property;
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }
}

var_dump($foo);

$foo = new Foo();
foreach ($foo as &$property) {
    try {
        $property++;
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }
}

var_dump($foo);

foreach ($foo as &$property) {
    try {
        --$property;
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }
}

var_dump($foo);

foreach ($foo as &$property) {
    try {
        $property--;
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }
}

var_dump($foo);

?>
--EXPECTF--
Cannot increment a reference held by property Foo::$property3 of an immutable class
Cannot increment a reference held by property Foo::$property4 of an immutable class
object(Foo)#1 (4) {
  ["property1"]=>
  int(1)
  ["property2"]=>
  int(3)
  ["property3"]=>
  int(%d)
  ["property4"]=>
  &int(%d)
}
Cannot increment a reference held by property Foo::$property3 of an immutable class
Cannot increment a reference held by property Foo::$property4 of an immutable class
object(Foo)#2 (4) {
  ["property1"]=>
  int(1)
  ["property2"]=>
  int(3)
  ["property3"]=>
  int(%d)
  ["property4"]=>
  &int(%d)
}
Cannot decrement a reference held by property Foo::$property3 of an immutable class
Cannot decrement a reference held by property Foo::$property4 of an immutable class
object(Foo)#2 (4) {
  ["property1"]=>
  int(0)
  ["property2"]=>
  int(2)
  ["property3"]=>
  int(-%d)
  ["property4"]=>
  &int(-%d)
}
Cannot decrement a reference held by property Foo::$property3 of an immutable class
Cannot decrement a reference held by property Foo::$property4 of an immutable class
object(Foo)#2 (4) {
  ["property1"]=>
  int(-1)
  ["property2"]=>
  int(1)
  ["property3"]=>
  int(-%d)
  ["property4"]=>
  &int(-%d)
}
