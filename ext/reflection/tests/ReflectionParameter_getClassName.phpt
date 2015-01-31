--TEST--
Test ReflectionParameter::getClassName() usage.
--FILE--
<?php
class Foo {
  public function bar(Qux $qux) {}
}

$class = new ReflectionClass(Foo::class);
$method = $class->getMethod("bar");
$params = $method->getParameters();
var_dump($params[0]->getClassName());

--EXPECT--
string(3) "Qux"
