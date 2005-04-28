--TEST--
Bug #29015 (Incorrect behavior of member vars(non string ones)-numeric mem vars und others)
--FILE--
<?php
$a = new stdClass();
$x = "";
$a->$x = "string('')";
var_dump($a);
?>
--EXPECTF--
Fatal error: Cannot access empty property in %sbug29015.php on line 4
