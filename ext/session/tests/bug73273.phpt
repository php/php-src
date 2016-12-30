--TEST--
Bug #73273: session_unset() empties values from all variables in which is $_session stored
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
session_start();
$_SESSION['test'] = true;
$var = $_SESSION;
session_unset();
var_dump($var);
?>
--EXPECT--
array(1) {
  ["test"]=>
  bool(true)
}
