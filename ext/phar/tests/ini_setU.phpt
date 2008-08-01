--TEST--
Phar: test ini_set with readonly and require_hash enabled
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (version_compare(PHP_VERSION, "6.0", "<")) die("skip Unicode support required");
?>
--INI--
phar.require_hash=1
phar.readonly=1
--FILE--
<?php
var_dump(ini_set('phar.require_hash', 1));
var_dump(ini_set('phar.readonly', 1));
var_dump(ini_get('phar.require_hash'));
var_dump(ini_get('phar.readonly'));
var_dump(ini_set('phar.require_hash', 0));
var_dump(ini_set('phar.readonly', 0));
var_dump(ini_get('phar.require_hash'));
var_dump(ini_get('phar.readonly'));
__HALT_COMPILER();
?>
--EXPECT--
unicode(1) "1"
unicode(1) "1"
unicode(1) "1"
unicode(1) "1"
unicode(1) "1"
unicode(1) "1"
unicode(1) "1"
unicode(1) "1"
