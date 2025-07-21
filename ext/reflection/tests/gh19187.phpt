--TEST--
GH-19187: ReflectionNamedType::getName() should not include nullable type
--FILE--
<?php

class Foo {
    public string|null $bar {
        set(string|null $value) => $value;
    }
}

$reflProp = new ReflectionProperty(Foo::class, 'bar');
echo $reflProp->getType()->getName(), "\n";
echo $reflProp->getSettableType()->getName(), "\n";

?>
--EXPECT--
string
string
