--TEST--
ReflectionExtension::getConstants()
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php
$ext = new ReflectionExtension("standard");
$consts = $ext->getConstants();
var_dump($consts["CONNECTION_NORMAL"]);
?>
--EXPECT--	
int(0)

