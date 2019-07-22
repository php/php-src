--TEST--
ReflectionExtension::export()
--CREDITS--
Gerrit "Remi" te Sligte <remi@wolerized.com>
Leon Luijkx <leon@phpgg.nl>
--FILE--
<?php
ReflectionExtension::export("reflection", true);
ob_start();
ReflectionExtension::export("reflection", false);
$test = ob_get_clean();
var_dump(empty($test));
?>
==DONE==
--EXPECTF--
Deprecated: Function ReflectionExtension::export() is deprecated in %s on line %d
bool(false)
==DONE==
