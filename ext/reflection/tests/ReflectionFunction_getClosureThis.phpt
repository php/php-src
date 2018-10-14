--TEST--
Reflection::getClosureThis()
--SKIPIF--
<?php
if (!extension_loaded('reflection')) print 'skip';
?>
--FILE--
<?php
$closure = function($param) { return "this is a closure"; };
$rf = new ReflectionFunction($closure);
var_dump($rf->getClosureThis());
echo "Done!\n";
--EXPECTF--
NULL
Done!
