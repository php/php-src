--TEST--
ReflectionAttribute getFileName
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

if ($attribute->getFileName() === __FILE__) {
    echo "OK";
} else {
    echo "FAIL";
}

?>
--EXPECT--
OK
