--TEST--
ReflectionExtension::getFunctions()
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php
$ext = new ReflectionExtension("standard");
$funcs = $ext->getFunctions();
echo $funcs["sleep"]->getName();
?>
--EXPECT--	
sleep

