--TEST--
ReflectionFunction::getParameter()
--FILE--
<?php
function foo(string $bar) {}

$function = new ReflectionFunction('foo');
var_dump($function->getParameter(-1));
?>
--EXPECTF--
Fatal error: Uncaught ValueError: ReflectionFunctionAbstract::getParameter(): Argument #1 ($parameter) must be greater than or equal to 0 in %s:5
Stack trace:
#0 %s: ReflectionFunctionAbstract->getParameter(-1)
#1 {main}
  thrown in %s on line 5