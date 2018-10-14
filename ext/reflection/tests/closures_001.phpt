--TEST--
Reflection on closures
--FILE--
<?php

$closure = function($a, $b = 0) { };

$ro = new ReflectionObject($closure);
$rm = $ro->getMethod('__invoke');
var_dump($rm->getNumberOfParameters());
var_dump($rm->getNumberOfRequiredParameters());
$rms = $ro->getMethods();
foreach($rms as $rm) {
	if ($rm->getName() == '__invoke') {
		var_dump($rm->getNumberOfParameters());
		var_dump($rm->getNumberOfRequiredParameters());
	}
}

echo "---\n";

$rm = new ReflectionMethod($closure, '__invoke');
var_dump($rm->getName());
var_dump($rm->getNumberOfParameters());
var_dump($rm->getNumberOfRequiredParameters());

echo "---\n";

$rp = new ReflectionParameter(array($closure, '__invoke'), 0);
var_dump($rp->isOptional());
$rp = new ReflectionParameter(array($closure, '__invoke'), 1);
var_dump($rp->isOptional());
$rp = new ReflectionParameter(array($closure, '__invoke'), 'a');
var_dump($rp->isOptional());
$rp = new ReflectionParameter(array($closure, '__invoke'), 'b');
var_dump($rp->isOptional());

echo "---\n";

$rp = new ReflectionParameter($closure, 0);
var_dump($rp->isOptional());
$rp = new ReflectionParameter($closure, 1);
var_dump($rp->isOptional());
$rp = new ReflectionParameter($closure, 'a');
var_dump($rp->isOptional());
$rp = new ReflectionParameter($closure, 'b');
var_dump($rp->isOptional());

?>
===DONE===
--EXPECTF--
int(2)
int(1)
int(2)
int(1)
---
string(8) "__invoke"
int(2)
int(1)
---
bool(false)
bool(true)
bool(false)
bool(true)
---
bool(false)
bool(true)
bool(false)
bool(true)
===DONE===
