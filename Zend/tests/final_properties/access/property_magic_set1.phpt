--TEST--
Test that the __set() magic method works as expected when a final property is modified
--FILE--
<?php

class Foo
{
    final public array $properties = [
        "property1" => "foo",
        "property2" => null,
        "property3" => [],
    ];

    public function __set($name, $value): void
    {
        $this->properties[$name] = $value;
    }
}

$foo = new Foo();

try {
    $foo->property1 = "bar";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property2 = "baz";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property3 = "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property4 = "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump($foo->properties);

?>
--EXPECT--
Cannot modify final property Foo::$properties after initialization
Cannot modify final property Foo::$properties after initialization
Cannot modify final property Foo::$properties after initialization
Cannot modify final property Foo::$properties after initialization
array(3) {
  ["property1"]=>
  string(3) "foo"
  ["property2"]=>
  NULL
  ["property3"]=>
  array(0) {
  }
}
