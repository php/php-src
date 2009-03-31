--TEST--
Integer overflow
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
$s = sprintf("%d", PHP_INT_MAX);
var_dump(is_long(filter_var($s, FILTER_VALIDATE_INT)));

$s = sprintf("%.0f", PHP_INT_MAX+1);
var_dump(filter_var($s, FILTER_VALIDATE_INT));

$s = sprintf("%d", -PHP_INT_MAX);
var_dump(is_long(filter_var($s, FILTER_VALIDATE_INT)));
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
