--TEST--
finfo_file(): Testing file names
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

$fp = finfo_open();
var_dump(finfo_file($fp, "\0"));
var_dump(finfo_file($fp, ''));
var_dump(finfo_file($fp, NULL));
var_dump(finfo_file($fp, '.'));
var_dump(finfo_file($fp, '&'));

?>
--EXPECTF--
Warning: finfo_file(): Empty filename or path in %s on line %d
bool(false)

Warning: finfo_file(): Empty filename or path in %s on line %d
bool(false)

Warning: finfo_file(): Empty filename or path in %s on line %d
bool(false)
string(9) "directory"

Warning: finfo_file(): File or path not found '&' in %s on line %d
bool(false)
