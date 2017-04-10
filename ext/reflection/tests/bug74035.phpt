--TEST--
Bug #74035: getNumberOfRequiredParameters wrong for ReflectionClass::newInstance
--FILE--
<?php
$r = new ReflectionClass(ReflectionClass::class);
$m = $r->getMethod('newInstance');

echo $m->getNumberOfRequiredParameters();
?>
--EXPECT--
0
