--TEST--
Bug #79099 (OOB read in php_strip_tags_ex)
--FILE--
<?php
var_dump(strip_tags("<?\n\"\n"));
var_dump(strip_tags("<\0\n!\n"));
var_dump(strip_tags("<\0\n?\n"));
?>
--EXPECT--
string(0) ""
string(0) ""
string(0) ""
