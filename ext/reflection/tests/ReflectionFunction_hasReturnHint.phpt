--TEST--
Reflection::hasReturnHint()
--SKIPIF--
<?php
if (!extension_loaded('reflection') || !defined('PHP_VERSION_ID') || PHP_VERSION_ID < 50399) {
  print 'skip';
}
?>
--FILE-- 
<?php
class foo {}

$closure = function($param) : foo { return new foo(); };
$rf = new ReflectionFunction($closure);
var_dump($rf->hasReturnHint());
$closure = function($param) { return new foo(); };
$rf = new ReflectionFunction($closure);
var_dump($rf->hasReturnHint());
--EXPECT--
bool(true)
bool(false)
