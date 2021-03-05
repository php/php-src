--TEST--
Test ReflectionMethod::createFromMethodName()
--FILE--
<?php

class Foo
{
    public function bar()
    {
    }
}

class MyReflectionMethod extends ReflectionMethod
{
}

$m1 = MyReflectionMethod::createFromMethodName("Foo::bar");
$m2 = new MyReflectionMethod("Foo::bar");

var_dump($m1::class);
var_dump($m1 == $m2);

try {
    MyReflectionMethod::createFromMethodName("Foo::baz");
} catch (ReflectionException $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
string(18) "MyReflectionMethod"
bool(true)
Method Foo::baz() does not exist
