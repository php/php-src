--TEST--
Lazy objects: RFC example 003
--FILE--
<?php

class MyLazyClass
{
    private int $foo;

    public function __construct()
    {
        $reflector = new ReflectionClass(self::class);
        $reflector->resetAsLazyGhost($this, $this->initialize(...), ReflectionClass::SKIP_DESTRUCTOR);
    }

    public function initialize()
    {
        $this->foo = 123;
    }

    public function getFoo()
    {
        return $this->foo;
    }

    public function __destruct()
    {
        var_dump(__METHOD__);
    }
}

$object = new MyLazyClass();

var_dump($object);
var_dump($object->getFoo());
var_dump($object);

?>
==DONE==
--EXPECTF--
lazy ghost object(MyLazyClass)#%d (0) {
  ["foo":"MyLazyClass":private]=>
  uninitialized(int)
}
int(123)
object(MyLazyClass)#%d (1) {
  ["foo":"MyLazyClass":private]=>
  int(123)
}
==DONE==
string(23) "MyLazyClass::__destruct"
