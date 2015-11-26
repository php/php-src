--TEST--
Bug #68938 (json_decode() decodes empty string without indicating error)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
json_decode("");
var_dump(json_last_error());
?>
--EXPECT--
int(4)
