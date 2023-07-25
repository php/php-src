--TEST--
ReflectionFunction::getParameter()
--FILE--
<?php
$method = new ReflectionMethod('WeakReference', 'create');
var_dump($method->getParameter('Object'));
?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: The parameter specified by its name could not be found in %s:3
Stack trace:
#0 %s: ReflectionFunctionAbstract->getParameter('Object')
#1 {main}
  thrown in %s on line 3