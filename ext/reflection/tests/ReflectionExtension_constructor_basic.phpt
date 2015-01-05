--TEST--
ReflectionExtension::__construct()
--CREDITS--
Gerrit "Remi" te Sligte <remi@wolerized.com>
Leon Luijkx <leon@phpgg.nl>
--FILE--
<?php
$obj = new ReflectionExtension('reflection');
$test = $obj instanceof ReflectionExtension;
var_dump($test);
?>
==DONE==
--EXPECT--
bool(true)
==DONE==
