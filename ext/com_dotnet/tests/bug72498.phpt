--TEST--
Bug #72498 variant_date_from_timestamp null dereference
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("com_dotnet")) print "skip COM/.Net support not present";
?>
--FILE--
<?php

$v1 = PHP_INT_MAX;
var_dump(variant_date_from_timestamp($v1));
?>
--EXPECTF--
Warning: variant_date_from_timestamp(): Invalid timestamp %d in %sbug72498.php on line %d
bool(false)
