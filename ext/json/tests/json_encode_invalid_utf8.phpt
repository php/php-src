--TEST--
json_encode() invalid UTF8
--SKIPIF--
<?php
if (!extension_loaded("json")) print "skip";
?>
--FILE--
<?php
$str = "foo\xb0bar";
var_dump(json_encode($str));
var_dump(json_encode($str, JSON_INVALID_UTF8_IGNORE));
var_dump(json_encode($str, JSON_INVALID_UTF8_SUBSTITUTE));
var_dump(json_encode($str, JSON_UNESCAPED_UNICODE));
var_dump(json_encode($str, JSON_UNESCAPED_UNICODE | JSON_INVALID_UTF8_SUBSTITUTE));
echo "Done\n";
?>
--EXPECTF--
bool(false)
string(8) ""foobar""
string(14) ""foo\ufffdbar""
bool(false)
string(9) "%s"
Done
