--TEST--
ReflectionExtension::__construct()
--CREDITS--
Gerrit "Remi" te Sligte <remi@wolerized.com>
Leon Luijkx <leon@phpgg.nl>
--FILE--
<?php
$obj = new ReflectionExtension();
$test = $obj instanceof ReflectionExtension;
var_dump($test);
?>
==DONE==
--EXPECTF--
Warning: ReflectionExtension::__construct() expects exactly %d parameter, %d given in %s.php on line %d
bool(true)
==DONE==
