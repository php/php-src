--TEST--
bug 8315, NULL values halt the validation
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

$var="3".chr(0)."foo";
var_dump(filter_var($var, FILTER_VALIDATE_INT));
$var="3".chr(0)."foo";
var_dump(filter_var($var, FILTER_VALIDATE_FLOAT));
?>
--EXPECT--
bool(false)
bool(false)
