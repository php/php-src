--TEST--
ReflectionExtension::isTemporary()
--FILE--
<?php
$obj = new ReflectionExtension('reflection');
var_dump($obj->isTemporary());
?>
--EXPECT--
bool(false)
