--TEST--
ReflectionExtension::getName()
--CREDITS--
Gerrit "Remi" te Sligte <remi@wolerized.com>
Leon Luijkx <leon@phpgg.nl>
--FILE--
<?php
$obj = new ReflectionExtension('reflection');
var_dump($obj->getName());
?>
==DONE==
--EXPECTF--
%s(10) "Reflection"
==DONE==
