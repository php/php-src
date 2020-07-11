--TEST--
ReflectionExtension::getConstants()
--FILE--
<?php
$ext = new ReflectionExtension("standard");
$consts = $ext->getConstants();
var_dump($consts["CONNECTION_NORMAL"]);
?>
--EXPECT--
int(0)
