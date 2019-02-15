--TEST--
Reflection on closures: Segfaults with getParameters() and getDeclaringFunction()
--FILE--
<?php

$closure = function($a, $b = 0) { };

$method = new ReflectionMethod ($closure, '__invoke');
$params = $method->getParameters ();
unset ($method);
$method = $params[0]->getDeclaringFunction ();
unset ($params);
echo $method->getName ()."\n";

$parameter = new ReflectionParameter (array ($closure, '__invoke'), 'b');
$method = $parameter->getDeclaringFunction ();
unset ($parameter);
echo $method->getName ()."\n";

?>
===DONE===
--EXPECT--
__invoke
__invoke
===DONE===
