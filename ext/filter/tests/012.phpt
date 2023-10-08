--TEST--
filter_input()
--EXTENSIONS--
filter
--FILE--
<?php

var_dump(filter_input(INPUT_GET, "test"));
var_dump(filter_input(INPUT_POST, "test"));
var_dump(filter_input(INPUT_COOKIE, ""));

var_dump(filter_input(INPUT_GET, "test", FILTER_DEFAULT, FILTER_NULL_ON_FAILURE));
var_dump(filter_input(INPUT_POST, "test", FILTER_DEFAULT, FILTER_NULL_ON_FAILURE));
var_dump(filter_input(INPUT_COOKIE, "", FILTER_DEFAULT, FILTER_NULL_ON_FAILURE));

echo "Done\n";
?>
--EXPECT--
NULL
NULL
NULL
bool(false)
bool(false)
bool(false)
Done
