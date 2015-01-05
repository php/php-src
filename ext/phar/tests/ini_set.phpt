--TEST--
Phar: test ini_set with readonly and require_hash enabled
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
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
if (version_compare(PHP_VERSION, "5.3", "<")) {
var_dump(false, false);
} else {
var_dump(ini_set('phar.require_hash', 0));
var_dump(ini_set('phar.readonly', 0));
}
var_dump(ini_get('phar.require_hash'));
var_dump(ini_get('phar.readonly'));
__HALT_COMPILER();
?>
--EXPECTF--
%unicode|string%(1) "1"
%unicode|string%(1) "1"
%unicode|string%(1) "1"
%unicode|string%(1) "1"
bool(false)
bool(false)
%unicode|string%(1) "1"
%unicode|string%(1) "1"
