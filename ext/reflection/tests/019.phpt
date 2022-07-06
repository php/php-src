--TEST--
ReflectionFunction::getExtensionName
--FILE--
<?php
$f = new ReflectionFunction("sleep");
var_dump($f->getExtensionName());
?>
--EXPECT--
string(8) "standard"
