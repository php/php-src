--TEST--
Bug #31436 (isset incorrectly returning true when passed a bad type)
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

