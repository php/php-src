--TEST--
ReflectionAttribute getLines
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

var_dump($rm->getStartLine());
var_dump($attribute->getLine());

?>
--EXPECT--
int(7)
int(6)
