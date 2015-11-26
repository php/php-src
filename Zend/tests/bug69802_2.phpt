--TEST--
Bug #69802 (Reflection on Closure::__invoke borks type hint class name)
--FILE--
<?php
$f = (new ReflectionFunction('iterator_to_array'))->getClosure();
$r = new ReflectionMethod($f, '__invoke');
var_dump($r->getParameters()[0]->getClass());
?>
--EXPECT--
object(ReflectionClass)#4 (1) {
  ["name"]=>
  string(11) "Traversable"
}
