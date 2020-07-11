--TEST--
ReflectionExtension::getFunctions()
--FILE--
<?php
$ext = new ReflectionExtension("standard");
$funcs = $ext->getFunctions();
echo $funcs["sleep"]->getName();
?>
--EXPECT--
sleep
