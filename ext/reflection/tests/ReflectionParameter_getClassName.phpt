--TEST--
Test ReflectionParameter::getClassName() usage.
--FILE--
<?php

use Bar\Baz;

class Foo {
  public function bar(Qux $qux, $bar, Baz $baz, \Bar\Quz $quz) {}

  public function waldo(array $wibble, callable $wobble) {}
}

$class = new ReflectionClass(Foo::class);
$method = $class->getMethod("bar");
$params = $method->getParameters();
var_dump($params[0]->getClassName());
var_dump($params[1]->getClassName());
var_dump($params[2]->getClassName());
var_dump($params[3]->getClassName());

$method = $class->getMethod("waldo");
$params = $method->getParameters();
var_dump($params[0]->getClassName());
var_dump($params[1]->getClassName());


--EXPECT--
string(3) "Qux"
NULL
string(7) "Bar\Baz"
string(7) "Bar\Quz"
NULL
NULL
