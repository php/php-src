--TEST--
Phar object: ArrayAccess and isset
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php

$pharconfig = 0;

require_once 'files/phar_oo_test.inc';

$phar = new Phar($fname);

var_dump(isset($phar['a.php']));
var_dump(isset($phar['b.php']));
var_dump(isset($phar['b/c.php']));
var_dump(isset($phar['b/d.php']));
var_dump(isset($phar['e.php']));

?>
===DIR===
<?php
var_dump(isset($phar['b']));
?>
===NA===
<?php
var_dump(isset($phar['a']));
var_dump(isset($phar['b/c']));
var_dump(isset($phar[12]));
var_dump(isset($phar['b']));

?>
--CLEAN--
<?php
unlink(__DIR__ . '/files/phar_oo_010.phar.php');
__halt_compiler();
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
===DIR===
bool(true)
===NA===
bool(false)
bool(false)
bool(false)
bool(true)
