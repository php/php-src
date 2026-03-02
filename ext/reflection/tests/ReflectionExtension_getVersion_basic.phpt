--TEST--
ReflectionExtension::getVersion()
--CREDITS--
Gerrit "Remi" te Sligte <remi@wolerized.com>
Leon Luijkx <leon@phpgg.nl>
--FILE--
<?php
$obj = new ReflectionExtension('reflection');
$var = $obj->getVersion() ? $obj->getVersion() : null;
var_dump($var);
?>
--EXPECTF--
string(%d) "%d.%d.%s"
