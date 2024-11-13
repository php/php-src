--TEST--
GH-13970 (Incorrect validation of #[\Attribute]'s first parameter)
--EXTENSIONS--
zend_test
--FILE--
<?php
#[Attribute(\ZendTestUnitEnum::Foo)]
class Foo {}

#[Foo]
function test() {}

$reflection = new ReflectionFunction('test');

try {
    var_dump($reflection->getAttributes()[0]->newInstance());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Attribute::__construct(): Argument #1 ($flags) must be of type int, ZendTestUnitEnum given
