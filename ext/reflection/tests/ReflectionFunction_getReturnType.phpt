--TEST--
Reflection::getReturnType()
--SKIPIF--
<?php
if (!extension_loaded('reflection') || !defined('PHP_VERSION_ID') || PHP_VERSION_ID < 50300) {
  print 'skip';
}
?>
--FILE-- 
<?php
$closure = function($param) : array { return []; };
$rf = new ReflectionFunction($closure);
var_dump($rf->getReturnType());
$closure = function($param) : callable { return $this; };
$rf = new ReflectionFunction($closure);
var_dump($rf->getReturnType());
$closure = function($param) : stdClass { return new stdClass; };
$rf = new ReflectionFunction($closure);
var_dump($rf->getReturnType());
--EXPECTF--
string(5) "array"
string(8) "callable"
object(ReflectionClass)#%d (%d) {
  ["name"]=>
  string(8) "stdClass"
}


