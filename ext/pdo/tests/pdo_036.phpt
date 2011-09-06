--TEST--
Testing PDORow and PDOStatement instances with Reflection
--SKIPIF--
<?php if (!extension_loaded('pdo')) die('skip'); ?>
--FILE--
<?php

$instance = new reflectionclass('pdorow');
$x = $instance->newInstance();
var_dump($x);

$instance = new reflectionclass('pdostatement');
$x = $instance->newInstance();
var_dump($x);

?>
--EXPECTF--
object(PDORow)#%d (0) {
}
object(PDOStatement)#%d (1) {
  [%u|b%"queryString"]=>
  NULL
}
