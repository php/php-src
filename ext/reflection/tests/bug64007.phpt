--TEST--
Bug #64007 (There is an ability to create instance of Generator by hand)
--FILE--
<?php
$reflection = new ReflectionClass('Generator');
$generator  = $reflection->newInstance();
var_dump($generator);
?>
--EXPECTF--
Catchable fatal error: The "Generator" class is reserved for internal use and cannot be manually instantiated in %sbug64007.php on line %d
