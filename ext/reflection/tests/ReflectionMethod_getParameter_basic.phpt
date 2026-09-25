--TEST--
ReflectionMethod::getParameter()
--FILE--
<?php
class C {
    public function foo(string $bar) {}
}

$method = new ReflectionMethod('WeakReference', 'create');
var_dump($method->getParameter('object'));
var_dump($method->getParameter(0));

$method = new ReflectionMethod('C', 'foo');
var_dump($method->getParameter('bar'));
var_dump($method->getParameter(0));
?>
--EXPECT--
object(ReflectionParameter)#2 (1) {
  ["name"]=>
  string(6) "object"
}
object(ReflectionParameter)#2 (1) {
  ["name"]=>
  string(6) "object"
}
object(ReflectionParameter)#1 (1) {
  ["name"]=>
  string(3) "bar"
}
object(ReflectionParameter)#1 (1) {
  ["name"]=>
  string(3) "bar"
}
