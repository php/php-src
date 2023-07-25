--TEST--
ReflectionFunction::getParameter()
--FILE--
<?php
function foo(string $bar) {}

$function = new ReflectionFunction('foo');
var_dump($function->getParameter('Bar'));
?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: The parameter specified by its name could not be found in %s:5
Stack trace:
#0 %s: ReflectionFunctionAbstract->getParameter('Bar')
#1 {main}
  thrown in %s on line 5