--TEST--
Reflection::hasReturnType()
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
var_dump($rf->hasReturnType());
$closure = function($param) { return $this; };
$rf = new ReflectionFunction($closure);
var_dump($rf->hasReturnType());
--EXPECT--
bool(true)
bool(false)


