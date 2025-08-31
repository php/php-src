--TEST--
Bug #47745 (FILTER_VALIDATE_INT doesn't allow minimum integer)
--EXTENSIONS--
filter
--FILE--
<?php
$s = (string)(-PHP_INT_MAX-1);
var_dump(intval($s));
var_dump(filter_var($s, FILTER_VALIDATE_INT));
?>
--EXPECTF--
int(-%d)
int(-%d)
