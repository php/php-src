--TEST--
ReflectionExtension::isPersistent()
--FILE--
<?php
$obj = new ReflectionExtension('reflection');
var_dump($obj->isPersistent());
?>
--EXPECT--
bool(true)
