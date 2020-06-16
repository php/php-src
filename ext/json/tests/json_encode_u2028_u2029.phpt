--TEST--
json_encode() tests for U+2028, U+2029
--FILE--
<?php
var_dump(json_encode(array("a\xC3\xA1b")));
var_dump(json_encode(array("a\xC3\xA1b"), JSON_UNESCAPED_UNICODE));
var_dump(json_encode("a\xE2\x80\xA7b"));
var_dump(json_encode("a\xE2\x80\xA7b", JSON_UNESCAPED_UNICODE));
var_dump(json_encode("a\xE2\x80\xA8b"));
var_dump(json_encode("a\xE2\x80\xA8b", JSON_UNESCAPED_UNICODE));
var_dump(json_encode("a\xE2\x80\xA8b", JSON_UNESCAPED_LINE_TERMINATORS));
var_dump(json_encode("a\xE2\x80\xA8b", JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_LINE_TERMINATORS));
var_dump(json_encode("a\xE2\x80\xA9b"));
var_dump(json_encode("a\xE2\x80\xA9b", JSON_UNESCAPED_UNICODE));
var_dump(json_encode("a\xE2\x80\xA9b", JSON_UNESCAPED_LINE_TERMINATORS));
var_dump(json_encode("a\xE2\x80\xA9b", JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_LINE_TERMINATORS));
var_dump(json_encode("a\xE2\x80\xAAb"));
var_dump(json_encode("a\xE2\x80\xAAb", JSON_UNESCAPED_UNICODE));
?>
--EXPECT--
string(12) "["a\u00e1b"]"
string(8) "["aáb"]"
string(10) ""a\u2027b""
string(7) ""a‧b""
string(10) ""a\u2028b""
string(10) ""a\u2028b""
string(10) ""a\u2028b""
string(7) ""a b""
string(10) ""a\u2029b""
string(10) ""a\u2029b""
string(10) ""a\u2029b""
string(7) ""a b""
string(10) ""a\u202ab""
string(7) ""a‪b""
