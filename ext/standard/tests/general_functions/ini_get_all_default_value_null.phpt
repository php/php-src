--TEST--
ini_get_all() reports a null default_value for a directive that has no compiled-in default
--INI--
error_append_string=FOO
--FILE--
<?php

// error_append_string is configured above, but it has no compiled-in default,
// so default_value must be null regardless of configuration or runtime changes.
$all = ini_get_all(null, true);

var_dump($all["error_append_string"]["global_value"]);
var_dump($all["error_append_string"]["local_value"]);
var_dump($all["error_append_string"]["default_value"]);

ini_set("error_append_string", "BAR");

$all = ini_get_all(null, true);

var_dump($all["error_append_string"]["global_value"]);
var_dump($all["error_append_string"]["local_value"]);
var_dump($all["error_append_string"]["default_value"]);

echo "Done\n";
?>
--EXPECT--
string(3) "FOO"
string(3) "FOO"
NULL
string(3) "FOO"
string(3) "BAR"
NULL
Done
