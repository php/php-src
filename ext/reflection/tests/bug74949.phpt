--TEST--
Bug #74949 (null pointer dereference in _function_string)
--FILE--
<?php

$f = function () {};

$r = new ReflectionMethod($f, "__invoke");

unset($f);

echo $r, "\n";

try  {
	echo $r->getPrototype();
} catch (Exception $e) {
	echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Method [ <internal> public method __invoke ] {
}

Method Closure::__invoke does not have a prototype
