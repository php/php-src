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
var_dump(filter_has_var(INPUT_GET, array()));

var_dump(filter_has_var(INPUT_POST, "b"));
var_dump(filter_has_var(INPUT_POST, "bbc"));
var_dump(filter_has_var(INPUT_POST, "nonex"));
var_dump(filter_has_var(INPUT_POST, " "));
var_dump(filter_has_var(INPUT_POST, ""));
var_dump(filter_has_var(INPUT_POST, array()));

var_dump(filter_has_var(-1, ""));
var_dump(filter_has_var("", ""));
var_dump(filter_has_var(array(), array()));
var_dump(filter_has_var(array(), ""));
var_dump(filter_has_var("", array()));

echo "Done\n";
?>
--EXPECTF--
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)

Warning: filter_has_var() expects parameter 2 to be string, array given in %s007.php on line %d
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)

Warning: filter_has_var() expects parameter 2 to be string, array given in %s007.php on line %d
bool(false)
bool(false)

Warning: filter_has_var() expects parameter 1 to be integer, string given in %s007.php on line %d
bool(false)

Warning: filter_has_var() expects parameter 1 to be integer, array given in %s007.php on line %d
bool(false)

Warning: filter_has_var() expects parameter 1 to be integer, array given in %s007.php on line %d
bool(false)

Warning: filter_has_var() expects parameter 1 to be integer, string given in %s007.php on line %d
bool(false)
Done
