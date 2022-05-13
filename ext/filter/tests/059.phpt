--TEST--
filter_var() and FILTER_SANITIZE_ADD_SLASHES
--EXTENSIONS--
filter
--FILE--
<?php

var_dump(filter_var("test'asd'asd'' asd\'\"asdfasdf", FILTER_SANITIZE_ADD_SLASHES));
var_dump(filter_var("'", FILTER_SANITIZE_ADD_SLASHES));
var_dump(filter_var("", FILTER_SANITIZE_ADD_SLASHES));
var_dump(filter_var(-1, FILTER_SANITIZE_ADD_SLASHES));

echo "Done\n";
?>
--EXPECT--
string(36) "test\'asd\'asd\'\' asd\\\'\"asdfasdf"
string(2) "\'"
string(0) ""
string(2) "-1"
Done
