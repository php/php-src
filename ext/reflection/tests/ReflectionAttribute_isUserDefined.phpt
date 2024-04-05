--TEST--
ReflectionAttribute isUserDefined
--FILE--
<?php
#[Attribute]
class A {}

class Foo {
    #[A]
    public function bar() {}
}

$rm = new ReflectionMethod(Foo::class, "bar");
$attribute = $rm->getAttributes()[0];

var_dump($attribute->isUserDefined());

?>
--EXPECT--
bool(true)
