--TEST--
Reflection Bug #26695 (Reflection API does not recognize mixed-case class hints)
--FILE--
<?php

class Foo {
}

class Bar {
  function demo(foo $f) {
  }
}

$class = new ReflectionClass('bar');
$methods = $class->getMethods();
$params = $methods[0]->getParameters();

$class = $params[0]->getClass();

var_dump($class->getName());
?>
===DONE===
--EXPECT--
string(3) "Foo"
===DONE===
