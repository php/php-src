--TEST--
Bug #69202 (FILTER_FLAG_STRIP_BACKTICK ignored unless other flags are used)
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_var("``a`b`c``", FILTER_UNSAFE_RAW, FILTER_FLAG_STRIP_BACKTICK));
var_dump(filter_var("``a`b`c``", FILTER_UNSAFE_RAW, FILTER_FLAG_STRIP_LOW | FILTER_FLAG_STRIP_BACKTICK));
var_dump(filter_var("``a`b`c``", FILTER_UNSAFE_RAW, FILTER_FLAG_STRIP_HIGH | FILTER_FLAG_STRIP_BACKTICK));
var_dump(filter_var("``a`b`c``", FILTER_UNSAFE_RAW, FILTER_FLAG_STRIP_LOW | FILTER_FLAG_STRIP_HIGH));
?>
--EXPECT--
string(3) "abc"
string(3) "abc"
string(3) "abc"
string(9) "``a`b`c``"
