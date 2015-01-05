--TEST--
ReflectionExtension::getVersion()
--CREDITS--
Gerrit "Remi" te Sligte <remi@wolerized.com>
Leon Luijkx <leon@phpgg.nl>
--FILE--
<?php
$obj = new ReflectionExtension('reflection');
$var = $obj->getVersion() ? $obj->getVersion() : null;
$test = floatval($var) == $var ? true : false;
var_dump($test);
?>
==DONE==
--EXPECT--
bool(true)
==DONE==
