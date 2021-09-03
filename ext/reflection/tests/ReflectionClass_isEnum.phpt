--TEST--
Testing ReflectionClass::isEnum()
--FILE--
<?php

class Foo {}
enum Bar {
    case Baz;
}

$fooReflection = new ReflectionClass(Foo::class);
$barReflection = new ReflectionClass(Bar::class);

var_dump($fooReflection->isEnum());
var_dump($barReflection->isEnum());

?>
--EXPECT--
bool(false)
bool(true)
