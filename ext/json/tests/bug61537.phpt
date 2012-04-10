--TEST--
Bug #61537 (json_encode() incorrectly truncates/discards information)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
$a = null;
$b = "\xff";

var_dump(json_encode($a), json_last_error(), json_encode($b), json_last_error());
?>
--EXPECT--
string(4) "null"
int(0)
bool(false)
int(5)
