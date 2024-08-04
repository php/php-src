--TEST--
Tidy::diagnose() NULL pointer dereference
--EXTENSIONS--
tidy
--FILE--
<?php

$nx = new Tidy();
$nx->diagnose();
var_dump($nx);

?>
--EXPECTF--
object(tidy)#%d (2) {
  ["errorBuffer"]=>
  NULL
  ["value"]=>
  NULL
}
