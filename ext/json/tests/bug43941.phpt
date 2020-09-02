--TEST--
Bug #43941 (json_encode() invalid UTF-8)
--FILE--
<?php

var_dump(json_encode("abc"));
var_dump(json_encode("ab\xE0"));
var_dump(json_encode("ab\xE0", JSON_PARTIAL_OUTPUT_ON_ERROR));
var_dump(json_encode(array("ab\xE0", "ab\xE0c", "abc"), JSON_PARTIAL_OUTPUT_ON_ERROR));

echo "Done\n";
?>
--EXPECT--
string(5) ""abc""
bool(false)
string(4) "null"
string(17) "[null,null,"abc"]"
Done
