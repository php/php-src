--TEST--
Phar: test ini_set with readonly and require_hash disabled
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (version_compare(PHP_VERSION, "6.0", "==")) die("skip pre-unicode version of PHP required");
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
string(1) "0"
string(1) "0"
string(1) "1"
string(1) "1"
bool(true)
yes
string(1) "0"
string(1) "0"
string(3) "yes"
string(3) "yes"
bool(false)
on
string(1) "0"
string(1) "0"
string(2) "on"
string(2) "on"
bool(false)
true
string(1) "0"
string(1) "0"
bool(false)
string(4) "true"
string(4) "true"
0
string(4) "true"
string(4) "true"
bool(true)
string(1) "0"
string(1) "0"
===DONE===
