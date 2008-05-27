--TEST--
ReflectionFunction::getExtensionName
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php
$f = new ReflectionFunction("sleep");
var_dump($f->getExtensionName());
?>
--EXPECT--	
unicode(8) "standard"
