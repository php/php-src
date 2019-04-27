--TEST--
stream_filter_register() and invalid arguments
--FILE--
<?php

var_dump(stream_filter_register("", ""));
var_dump(stream_filter_register("test", ""));
var_dump(stream_filter_register("", "test"));
var_dump(stream_filter_register("------", "nonexistentclass"));

echo "Done\n";
?>
--EXPECTF--
Warning: stream_filter_register(): Filter name cannot be empty in %s on line %d
bool(false)

Warning: stream_filter_register(): Class name cannot be empty in %s on line %d
bool(false)

Warning: stream_filter_register(): Filter name cannot be empty in %s on line %d
bool(false)
bool(true)
Done
