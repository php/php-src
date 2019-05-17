--TEST--
filter_var() and FILTER_SANITIZE_ADD_SLASHES
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

function filter_test_compare($input) {
    return filter_var($input, FILTER_SANITIZE_ADD_SLASHES) === filter_var($input, FILTER_SANITIZE_MAGIC_QUOTES);
}

var_dump(filter_var("test'asd'asd'' asd\'\"asdfasdf", FILTER_SANITIZE_ADD_SLASHES));
var_dump(filter_var("'", FILTER_SANITIZE_ADD_SLASHES));
var_dump(filter_var("", FILTER_SANITIZE_ADD_SLASHES));
var_dump(filter_var(-1, FILTER_SANITIZE_ADD_SLASHES));

var_dump(filter_test_compare("test'asd'asd'' asd\'\"asdfasdf"));
var_dump(filter_test_compare("''"));
var_dump(filter_test_compare(''));
var_dump(filter_test_compare(-1));

echo "Done\n";
?>
--EXPECT--
string(36) "test\'asd\'asd\'\' asd\\\'\"asdfasdf"
string(2) "\'"
string(0) ""
string(2) "-1"
bool(true)
bool(true)
bool(true)
bool(true)
Done
