--TEST--
Lazy objects: RFC example 002
--FILE--
<?php

class MyClass
{
    public function __construct(private int $foo)
    {
        // Heavy initialization logic here.
    }

    public function getFoo()
    {
        return $this->foo;
    }
}

$reflector = new ReflectionClass(MyClass::class);

$initializer = static function (MyClass $proxy): MyClass {
    return new MyClass(123);
};

$object = $reflector->newLazyProxy($initializer);

var_dump($object);
var_dump($object->getFoo());
var_dump($object);

?>
--EXPECTF--
lazy proxy object(MyClass)#%d (0) {
  ["foo":"MyClass":private]=>
  uninitialized(int)
}
int(123)
lazy proxy object(MyClass)#%d (1) {
  ["instance"]=>
  object(MyClass)#%d (1) {
    ["foo":"MyClass":private]=>
    int(123)
  }
}
