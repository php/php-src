--TEST--
Test typed properties type applies to all props in group
--FILE--
<?php
class Foo {
    public int $bar,
                $qux;
}

$reflector = new ReflectionClass(Foo::class);

$prop = $reflector->getProperty("qux");

var_dump($prop->getType()->getName());
?>
--EXPECT--
string(3) "int"
