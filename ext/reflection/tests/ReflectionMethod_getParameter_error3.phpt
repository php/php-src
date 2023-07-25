--TEST--
ReflectionFunction::getParameter()
--FILE--
<?php
$method = new ReflectionMethod('WeakReference', 'create');
var_dump($method->getParameter(3));
?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: The parameter specified by its offset could not be found in %s:3
Stack trace:
#0 %s: ReflectionFunctionAbstract->getParameter(3)
#1 {main}
  thrown in %s on line 3