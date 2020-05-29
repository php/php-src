--TEST--
Test that the mixed type is reflectable
--FILE--
<?php
class A
{
    public mixed $a;

    public function test(mixed ...$a): mixed {}
}

$a = new A();

$object = new ReflectionObject($a);
$method = new ReflectionMethod($a, "test");

var_dump($object->getProperty("a")->getType()->getName());
var_dump($method->getParameters()[0]->getType()->getName());
var_dump($method->getReturnType()->getName());

var_dump((string) $object->getProperty("a")->getType());
var_dump((string) $method->getParameters()[0]->getType());
var_dump((string) $method->getReturnType());

?>
--EXPECT--
string(5) "mixed"
string(5) "mixed"
string(5) "mixed"
string(5) "mixed"
string(5) "mixed"
string(5) "mixed"
