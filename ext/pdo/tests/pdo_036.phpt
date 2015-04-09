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
  [%u|b%"queryString"]=>
  NULL
}

Fatal error: PDORow::__construct(): You should not create a PDOStatement manually in %spdo_036.php on line %d
