--TEST--
ini_get_all() exposes the built-in default value independent of configuration and runtime changes
--INI--
precision=8
--FILE--
<?php

// precision is configured to 8 above, but its compiled-in default is 14.
$all = ini_get_all(null, true);

var_dump($all["precision"]["global_value"]);
var_dump($all["precision"]["local_value"]);
var_dump($all["precision"]["builtin_default_value"]);

// A runtime change must not affect builtin_default_value.
ini_set("precision", "3");

$all = ini_get_all(null, true);

var_dump($all["precision"]["global_value"]);
var_dump($all["precision"]["local_value"]);
var_dump($all["precision"]["builtin_default_value"]);

echo "Done\n";
?>
--EXPECT--
string(1) "8"
string(1) "8"
string(2) "14"
string(1) "8"
string(1) "3"
string(2) "14"
Done
