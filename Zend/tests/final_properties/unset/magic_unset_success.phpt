--TEST--
Test that the __unset() magic method is successful
--XFAIL--
Unsetting is currently not possible because of "Cannot modify final property Foo::$properties by reference after initialization"
--FILE--
<?php

class Foo
{
    final public ?array $properties;

    public function __isset($name): bool
    {
        return isset($this->properties[$name]);
    }

    public function __get($name)
    {
        return $this->properties[$name] ?? null;
    }

    public function __set($name, $value): void
    {
        $this->properties[$name] = $value;
    }

    public function __unset($name): void
    {
        unset($this->properties);
    }
}

$foo = new Foo();

unset($foo->property1);
var_dump($foo->properties);

?>
--EXPECT--
array(0) {
}
