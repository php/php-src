--TEST--
Bug #31098 (isset() / empty() incorrectly returns true in dereference of a wrong type)
--FILE--
<?php
$a = '';
var_dump(isset($a->b));
$a = 'a';
var_dump(isset($a->b));
$a = '0';
var_dump(isset($a->b));
$a = '';
var_dump(isset($a{'b'}));
$a = 'a';
var_dump(isset($a{'b'}));
$a = '0';
var_dump(isset($a{'b'}));
?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)

