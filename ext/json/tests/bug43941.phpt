--TEST--
Bug #43941 (json_encode() invalid UTF-8)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

var_dump(json_encode(b"abc"));
var_dump(json_encode(b"ab\xE0"));
var_dump(json_encode(b"ab\xE0c"));
var_dump(json_encode(array(b"ab\xE0", b"ab\xE0c", b"abc")));

echo "Done\n";
?>
--EXPECT--
string(5) ""abc""
string(4) "null"
string(4) "null"
string(17) "[null,null,"abc"]"
Done
