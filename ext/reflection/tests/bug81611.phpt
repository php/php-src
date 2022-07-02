--TEST--
Reflection Bug #81611 (ArgumentCountError when getting default value from ReflectionParameter with new)
--FILE--
<?php

class Bar
{
}

class Foo extends Bar
{
    public function doFoo(object $test = new self()): object
    {
        return $test;
    }

    public function doBar(object $test = new parent()): object
    {
        return $test;
    }
}

$ref = new \ReflectionClass(Foo::class);

foreach (['doFoo', 'doBar'] as $method) {
    $params = $ref->getMethod($method)->getParameters();

    foreach ($params as $param) {
        echo "isDefaultValueAvailable:\n";
        var_dump($param->isDefaultValueAvailable());

        echo "isDefaultValueConstant:\n";
        var_dump($param->isDefaultValueConstant());

        echo "getDefaultValueConstantName:\n";
        var_dump($param->getDefaultValueConstantName());

        echo "getDefaultValue:\n";
        var_dump($param->getDefaultValue());

        echo "\n";
    }
}
?>
--EXPECT--
isDefaultValueAvailable:
bool(true)
isDefaultValueConstant:
bool(false)
getDefaultValueConstantName:
NULL
getDefaultValue:
object(Foo)#2 (0) {
}

isDefaultValueAvailable:
bool(true)
isDefaultValueConstant:
bool(false)
getDefaultValueConstantName:
NULL
getDefaultValue:
object(Bar)#3 (0) {
}
