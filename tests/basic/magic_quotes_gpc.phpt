--TEST--
Bug #61043 (Regression in magic_quotes_gpc fix for CVE-2012-0831)
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
magic_quotes_gpc=On
--FILE--
<?php
var_dump(ini_get("magic_quotes_gpc"));
?>
--EXPECT--
string(1) "1"
