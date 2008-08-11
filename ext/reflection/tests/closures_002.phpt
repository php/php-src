--TEST--
Reflection on invokable objects
--FILE-- 
<?php

class Test {
	function __invoke($a, $b = 0) { }
}

$rm = new ReflectionMethod(new Test);
var_dump($rm->getName());
var_dump($rm->getNumberOfParameters());
var_dump($rm->getNumberOfRequiredParameters());

$rp = new ReflectionParameter(new Test, 0);
var_dump($rp->isOptional());

$rp = new ReflectionParameter(new Test, 1);
var_dump($rp->isOptional());

?>
===DONE===
--EXPECTF--
unicode(8) "__invoke"
int(2)
int(1)
bool(false)
bool(true)
===DONE===
