--TEST--
json_encode() invalid UTF8
--SKIPIF--
<?php
if (!extension_loaded("json")) print "skip";
?>
--FILE--
<?php
function json_encode_invalid_utf8($str) {
	var_dump(json_encode($str));
	var_dump(json_encode($str, JSON_INVALID_UTF8_IGNORE));
	var_dump(json_encode($str, JSON_INVALID_UTF8_SUBSTITUTE));
	var_dump(json_encode($str, JSON_UNESCAPED_UNICODE));
	var_dump(bin2hex(json_encode($str, JSON_UNESCAPED_UNICODE | JSON_INVALID_UTF8_SUBSTITUTE)));
}
json_encode_invalid_utf8("a\xb0b");
json_encode_invalid_utf8("a\xd0\xf2b");
echo "Done\n";
?>
--EXPECT--
bool(false)
string(2) "ab"
string(8) "a\ufffdb"
bool(false)
string(12) "2261fffd6222"
bool(false)
string(2) "ab"
string(8) "a\ufffdb"
bool(false)
string(12) "2261fffd6222"
Done
