--TEST--
ReflectionExtension::export()
--CREDITS--
Gerrit "Remi" te Sligte <remi@wolerized.com>
Leon Luijkx <leon@phpgg.nl>
--FILE--
<?php
ob_start();
ReflectionExtension::export("reflection", true);
$test = ob_get_clean();
var_dump(empty($test));
unset($test);
ob_start();
ReflectionExtension::export("reflection", false);
$test = ob_get_clean();
var_dump(empty($test));
?>
==DONE==
--EXPECT--
bool(true)
bool(false)
==DONE==
