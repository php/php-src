--TEST--
Bug #46205 (Closure - Memory leaks when ReflectionException is thrown)
--FILE--
<?php
$x = new reflectionmethod('reflectionparameter', 'export');
$y = function() { };

try {
	$x->invokeArgs(new reflectionparameter('trim', 'str'), array($y, 1));
} catch (Exception $e) { }
?>
ok
--EXPECT--
ok
