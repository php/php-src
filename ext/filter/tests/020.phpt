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
--EXPECTF--
Deprecated: filter_var(): FILTER_SANITIZE_MAGIC_QUOTES is deprecated, use FILTER_SANITIZE_ADD_SLASHES instead in %s on line %d
string(36) "test\'asd\'asd\'\' asd\\\'\"asdfasdf"

Deprecated: filter_var(): FILTER_SANITIZE_MAGIC_QUOTES is deprecated, use FILTER_SANITIZE_ADD_SLASHES instead in %s on line %d
string(2) "\'"

Deprecated: filter_var(): FILTER_SANITIZE_MAGIC_QUOTES is deprecated, use FILTER_SANITIZE_ADD_SLASHES instead in %s on line %d
string(0) ""

Deprecated: filter_var(): FILTER_SANITIZE_MAGIC_QUOTES is deprecated, use FILTER_SANITIZE_ADD_SLASHES instead in %s on line %d
string(2) "-1"
Done
