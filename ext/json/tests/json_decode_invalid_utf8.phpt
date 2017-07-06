--TEST--
json_decode() invalid UTF8
--SKIPIF--
<?php
if (!extension_loaded("json")) print "skip";
?>
--FILE--
<?php
function json_decode_invalid_utf8($str) {
	var_dump(json_decode($str));
	var_dump(json_decode($str, true, 512, JSON_INVALID_UTF8_IGNORE));
	var_dump(bin2hex(json_decode($str, true, 512, JSON_INVALID_UTF8_SUBSTITUTE)));
}
json_decode_invalid_utf8("\"a\xb0b\"");
json_decode_invalid_utf8("\"a\xd0\xf2b\"");
echo "Done\n";
?>
--EXPECT--
NULL
string(2) "ab"
string(12) "efbfbd62"
NULL
string(2) "ab"
string(12) "efbfbd62"
Done
