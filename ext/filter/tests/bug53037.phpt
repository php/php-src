--TEST--
Bug #53037 (FILTER_FLAG_EMPTY_STRING_NULL is not implemented)
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(
    filter_var("", FILTER_DEFAULT),
    filter_var("", FILTER_DEFAULT, array('flags' => FILTER_FLAG_EMPTY_STRING_NULL))
);
?>
--EXPECT--
string(0) ""
NULL
