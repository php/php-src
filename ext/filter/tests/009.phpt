--TEST--
filter_id()
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

var_dump(filter_id("stripped"));
var_dump(filter_id("string"));
var_dump(filter_id("url"));
var_dump(filter_id("int"));
var_dump(filter_id("none"));
var_dump(filter_id(-1));

echo "Done\n";
?>
--EXPECT--
int(513)
int(513)
int(518)
int(257)
bool(false)
bool(false)
Done
