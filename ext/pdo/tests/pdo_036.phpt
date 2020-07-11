--TEST--
Testing PDORow and PDOStatement instances with Reflection
--SKIPIF--
<?php if (!extension_loaded('pdo')) die('skip'); ?>
--FILE--
<?php

$instance = new reflectionclass('pdostatement');
$x = $instance->newInstance();
var_dump($x);

$instance = new reflectionclass('pdorow');
$x = $instance->newInstance();
var_dump($x);

?>
--EXPECTF--
object(PDOStatement)#%d (1) {
  ["queryString"]=>
  NULL
}

Fatal error: Uncaught PDOException: You may not create a PDORow manually in %spdo_036.php:8
Stack trace:
#0 %spdo_036.php(8): ReflectionClass->newInstance()
#1 {main}
  thrown in %spdo_036.php on line 8
