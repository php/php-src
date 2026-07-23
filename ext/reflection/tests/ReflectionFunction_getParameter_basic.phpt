--TEST--
ReflectionFunction::getParameter()
--FILE--
<?php
function foo(string $bar) {}

$function = new ReflectionFunction('sort');
var_dump($function->getParameter('array'));
var_dump($function->getParameter('flags'));
var_dump($function->getParameter(0));
var_dump($function->getParameter(1));

$function = new ReflectionFunction('foo');
var_dump($function->getParameter('bar'));
var_dump($function->getParameter(0));
?>
--EXPECT--
object(ReflectionParameter)#2 (1) {
  ["name"]=>
  string(5) "array"
}
object(ReflectionParameter)#2 (1) {
  ["name"]=>
  string(5) "flags"
}
object(ReflectionParameter)#2 (1) {
  ["name"]=>
  string(5) "array"
}
object(ReflectionParameter)#2 (1) {
  ["name"]=>
  string(5) "flags"
}
object(ReflectionParameter)#1 (1) {
  ["name"]=>
  string(3) "bar"
}
object(ReflectionParameter)#1 (1) {
  ["name"]=>
  string(3) "bar"
}
