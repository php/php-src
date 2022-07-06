--TEST--
filter_has_var()
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--GET--
a=qwe&abc=<a>href</a>
--POST--
b=qwe&bbc=<a>href</a>
--FILE--
<?php

var_dump(filter_has_var(INPUT_GET, "a"));
var_dump(filter_has_var(INPUT_GET, "abc"));
var_dump(filter_has_var(INPUT_GET, "nonex"));
var_dump(filter_has_var(INPUT_GET, " "));
var_dump(filter_has_var(INPUT_GET, ""));

var_dump(filter_has_var(INPUT_POST, "b"));
var_dump(filter_has_var(INPUT_POST, "bbc"));
var_dump(filter_has_var(INPUT_POST, "nonex"));
var_dump(filter_has_var(INPUT_POST, " "));
var_dump(filter_has_var(INPUT_POST, ""));

echo "Done\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
Done
