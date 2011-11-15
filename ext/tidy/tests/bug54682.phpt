--TEST--
Tidy::diagnose() NULL pointer dereference
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php

$nx = new Tidy("*");
$nx->diagnose();

?>
--EXPECTF--
Warning: tidy::__construct(): Cannot Load '*' into memory in %s on line %d
