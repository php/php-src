--TEST--
GH-13970 (Incorrect validation of #[\Attribute]’s first parameter)
--EXTENSIONS--
zend_test
--FILE--
<?php
#[Attribute(\ZendTestUnitEnum::Foo)]
class Foo {

}

#[Foo]
function test1() {

}

$reflection = new ReflectionFunction('test1');
var_dump($reflection->getAttributes()[0]->newInstance());
?>
--EXPECTF--
Fatal error: Attribute::__construct(): Argument #1 ($flags) must be of type int, ZendTestUnitEnum given in %s on line %d
