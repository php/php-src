--TEST--
filter_has_var() tests
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--GET--
a=1&b=2&c=0
--POST--
ap[]=1&bp=test&cp=
--FILE--
<?php

var_dump(filter_has_var(INPUT_GET,""));
var_dump(filter_has_var(INPUT_POST, "ap"));
var_dump(filter_has_var(INPUT_POST, "cp"));
var_dump(filter_has_var(INPUT_GET, "a"));
var_dump(filter_has_var(INPUT_GET, "c"));
var_dump(filter_has_var(INPUT_GET, "abc"));
var_dump(filter_has_var(INPUT_GET, "cc"));
var_dump(filter_has_var(-1, "cc"));
var_dump(filter_has_var(0, "cc"));

echo "Done\n";
?>
--EXPECTF--
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)

Warning: filter_has_var(): Unknown source in %s on line %d
bool(false)
bool(false)
Done
