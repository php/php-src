--TEST--
Reflection on closures: Segfaults with getParameters() and getDeclaringFunction()
--FILE--
<?php

$closure = function($a, $b = 0) { };

$method = new ReflectionFunction ($closure);
$params = $method->getParameters ();
unset ($method);
$method = $params[0]->getDeclaringFunction ();
unset ($params);
echo $method->getName ()."\n";

$parameter = new ReflectionParameter ($closure, 'b');
$method = $parameter->getDeclaringFunction ();
unset ($parameter);
echo $method->getName ()."\n";

?>
===DONE===
--EXPECTF--
{closure}
{closure}
===DONE===
