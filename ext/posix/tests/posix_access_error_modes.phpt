--TEST--
Test posix_access() function test error conditions
--DESCRIPTION--
checks if posix_access() failes for wrong permissions
--CREDITS--
Moritz Neuhaeuser, info@xcompile.net
PHP Testfest Berlin 2009-05-10
--SKIPIF--
<?php
if (!extension_loaded('posix')) {
    die('SKIP The posix extension is not loaded.');
}
if (posix_geteuid() == 0) {
    die('SKIP Cannot run test as root.');
}
if (PHP_VERSION_ID < 503099) {
    die('SKIP Safe mode is no longer available.');
}
?>
--FILE--
<?php
$filename = dirname(__FILE__) . '/foo.test';
var_dump(posix_access($filename, POSIX_F_OK));
$fp = fopen($filename,"w");
fwrite($fp,"foo");
fclose($fp);

chmod ($filename, 0000);
var_dump(posix_access($filename, POSIX_R_OK));
var_dump(posix_access($filename, POSIX_W_OK));
var_dump(posix_access($filename, POSIX_X_OK));
?>
===DONE===
--CLEAN--
<?php
$filename = dirname(__FILE__) . '/foo.test';
chmod ($filename, 0700);
unlink($filename);
?>
--EXPECTF--
WDeprecated: Directive 'safe_mode' is deprecated in PHP 5.3 and greater in Unknown on line %d
bool(false)
bool(false)
bool(false)
bool(false)
===DONE===
