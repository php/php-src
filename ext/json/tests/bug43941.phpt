--TEST--
Bug #43941 (json_encode() invalid UTF-8)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

var_dump(json_encode("abc"));
var_dump(json_encode("ab\xE0"));
var_dump(json_encode("ab\xE0c"));
var_dump(json_encode(array("ab\xE0", "ab\xE0c", "abc")));

echo "Done\n";
?>
--EXPECTF--
string(5) ""abc""
string(4) "null"
string(4) "null"
string(17) "[null,null,"abc"]"
Done

