--TEST--
filter_var() and FILTER_SANITIZE_MAGIC_QUOTES
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

var_dump(filter_var("test'asd'asd'' asd\'\"asdfasdf", FILTER_SANITIZE_MAGIC_QUOTES));
var_dump(filter_var("'", FILTER_SANITIZE_MAGIC_QUOTES));
var_dump(filter_var("", FILTER_SANITIZE_MAGIC_QUOTES));
var_dump(filter_var(-1, FILTER_SANITIZE_MAGIC_QUOTES));

echo "Done\n";
?>
--EXPECT--	
string(36) "test\'asd\'asd\'\' asd\\\'\"asdfasdf"
string(2) "\'"
string(0) ""
string(2) "-1"
Done
