--TEST--
ReflectionAttribute cannot be instantiated directly
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

$rm = new ReflectionMethod($attribute, "__construct");

try {
    var_dump($rm->invoke($attribute, 0, 1, 2));
} catch (ReflectionException $exception) {
    echo $exception->getMessage();
}
?>
--EXPECT--
Cannot directly instantiate ReflectionAttribute
