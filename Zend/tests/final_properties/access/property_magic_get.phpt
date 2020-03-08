--TEST--
Test that the __get() magic method works when reading from a final property
--FILE--
<?php

class Foo
{
    final private array $properties = [
        "property1" => "foo",
        "property2" => null,
    ];

    public function __isset($name): bool
    {
        return isset($this->properties[$name]);
    }

    public function __get($name)
    {
        return $this->properties[$name] ?? null;
    }
}

$foo = new Foo();

var_dump(isset($foo->property1));
var_dump($foo->property1);

var_dump(isset($foo->property2));
var_dump($foo->property2);

var_dump(isset($foo->property3));
var_dump($foo->property3);

?>
--EXPECT--
bool(true)
string(3) "foo"
bool(false)
NULL
bool(false)
NULL
