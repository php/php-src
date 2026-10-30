--TEST--
Exception handling in ReflectionAttribute::newInstance()
--FILE--
<?php

#[\Attribute]
class A {
    public function __construct() {
        throw new \Exception('Test');
    }
}

class Foo {
    #[A]
    public function bar() {}
}

$rm = new ReflectionMethod(Foo::class, "bar");
$attribute = $rm->getAttributes()[0];

var_dump($attribute->newInstance());
?>
--EXPECTF--
Fatal error: Uncaught Exception: Test in %s:6
Stack trace:
#0 %sReflectionAttribute_newInstance_exception.php(11): A->__construct()
#1 %sReflectionAttribute_newInstance_exception.php(18): ReflectionAttribute->newInstance()
#2 {main}
  thrown in %sReflectionAttribute_newInstance_exception.php on line 6
