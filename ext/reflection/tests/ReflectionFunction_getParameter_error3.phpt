--TEST--
ReflectionFunction::getParameter()
--FILE--
<?php
$function = new ReflectionFunction('sort');
var_dump($function->getParameter(3));
?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: The parameter specified by its offset could not be found in %s:3
Stack trace:
#0 %s: ReflectionFunctionAbstract->getParameter(3)
#1 {main}
  thrown in %s on line 3