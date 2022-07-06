--TEST--
Bug #76359 (open_basedir bypass through adding "..")
--FILE--
<?php
ini_set('open_basedir', __DIR__);
mkdir(__DIR__ . "/bug76359");
chdir(__DIR__ . "/bug76359");
var_dump(ini_set('open_basedir', ini_get('open_basedir') . PATH_SEPARATOR . ".."));
chdir("..");
chdir("..");
?>
--EXPECTF--
bool(false)

Warning: chdir(): open_basedir restriction in effect. File(..) is not within the allowed path(s): (%s) in %s on line %d
--CLEAN--
<?php
@rmdir(__DIR__ . "/bug76359");
?>
