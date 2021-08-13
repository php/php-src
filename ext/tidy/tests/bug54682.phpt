--TEST--
Tidy::diagnose() NULL pointer dereference
--EXTENSIONS--
tidy
--FILE--
<?php

$nx = new Tidy("*");
$nx->diagnose();

?>
--EXPECTF--
Warning: tidy::__construct(): Cannot load "*" into memory%win %s on line %d
