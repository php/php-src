--TEST--
filter_data() and FILTER_SANITIZE_MAGIC_QUOTES
--FILE--
<?php

var_dump(filter_data("test'asd'asd'' asd\'\"asdfasdf", FILTER_SANITIZE_MAGIC_QUOTES));
var_dump(filter_data("'", FILTER_SANITIZE_MAGIC_QUOTES));
var_dump(filter_data("", FILTER_SANITIZE_MAGIC_QUOTES));
var_dump(filter_data(-1, FILTER_SANITIZE_MAGIC_QUOTES));

echo "Done\n";
?>
--EXPECT--	
string(36) "test\'asd\'asd\'\' asd\\\'\"asdfasdf"
string(2) "\'"
string(0) ""
string(2) "-1"
Done
