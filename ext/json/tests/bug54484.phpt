--TEST--
Bug #54484 (Empty string in json_decode doesn't reset json_last_error)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
json_decode('{"test":"test"}');
var_dump(json_last_error());

json_decode("");
var_dump(json_last_error());

json_decode("invalid json");
var_dump(json_last_error());

json_decode("\"\001 invalid json\"");
var_dump(json_last_error());

json_decode("");
var_dump(json_last_error());
?>
--EXPECT--
int(0)
int(4)
int(4)
int(3)
int(4)
