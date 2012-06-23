--TEST--
Bug #43941 (json_encode() invalid UTF-8)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

var_dump(json_encode("abc"));
var_dump(json_encode("ab\xE0"));
var_dump(json_encode("ab\xE0", JSON_PARTIAL_OUTPUT_ON_ERROR));
var_dump(json_encode(array("ab\xE0", "ab\xE0c", "abc"), JSON_PARTIAL_OUTPUT_ON_ERROR));

echo "Done\n";
?>
--EXPECTF--
string(5) ""abc""

Warning: json_encode(): Invalid UTF-8 sequence in argument in %s on line %d
bool(false)

Warning: json_encode(): Invalid UTF-8 sequence in argument in %s on line %d
string(4) "null"

Warning: json_encode(): Invalid UTF-8 sequence in argument in %s on line %d

Warning: json_encode(): Invalid UTF-8 sequence in argument in %s on line %d
string(17) "[null,null,"abc"]"
Done
