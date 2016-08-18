--TEST--
Phar: test ini_set with readonly and require_hash disabled
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (version_compare(PHP_VERSION, "6.0", "!=")) die("skip Unicode support required");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
var_dump(ini_set('phar.require_hash', 1));
var_dump(ini_set('phar.readonly', 1));
var_dump(ini_get('phar.require_hash'));
var_dump(ini_get('phar.readonly'));
ini_set('phar.require_hash', 0);
ini_set('phar.readonly', 0);
var_dump(Phar::canWrite());
?>
yes
<?php
var_dump(ini_set('phar.require_hash', 'yes'));
var_dump(ini_set('phar.readonly', 'yes'));
var_dump(ini_get('phar.require_hash'));
var_dump(ini_get('phar.readonly'));
var_dump(Phar::canWrite());
ini_set('phar.require_hash', 0);
ini_set('phar.readonly', 0);
?>
on
<?php
var_dump(ini_set('phar.require_hash', 'on'));
var_dump(ini_set('phar.readonly', 'on'));
var_dump(ini_get('phar.require_hash'));
var_dump(ini_get('phar.readonly'));
var_dump(Phar::canWrite());
ini_set('phar.require_hash', 0);
ini_set('phar.readonly', 0);
?>
true
<?php
var_dump(ini_set('phar.require_hash', 'true'));
var_dump(ini_set('phar.readonly', 'true'));
var_dump(Phar::canWrite());
var_dump(ini_get('phar.require_hash'));
var_dump(ini_get('phar.readonly'));
?>
0
<?php
var_dump(ini_set('phar.require_hash', 0));
var_dump(ini_set('phar.readonly', 0));
var_dump(Phar::canWrite());
var_dump(ini_get('phar.require_hash'));
var_dump(ini_get('phar.readonly'));
?>
===DONE===
--EXPECT--
unicode(1) "0"
unicode(1) "0"
unicode(1) "1"
unicode(1) "1"
bool(true)
yes
unicode(1) "0"
unicode(1) "0"
unicode(3) "yes"
unicode(3) "yes"
bool(false)
on
unicode(1) "0"
unicode(1) "0"
unicode(2) "on"
unicode(2) "on"
bool(false)
true
unicode(1) "0"
unicode(1) "0"
bool(false)
unicode(4) "true"
unicode(4) "true"
0
unicode(4) "true"
unicode(4) "true"
bool(true)
unicode(1) "0"
unicode(1) "0"
===DONE===
