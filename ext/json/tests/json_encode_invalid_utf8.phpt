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
json_encode_invalid_utf8("\x61\xb0\x62");
json_encode_invalid_utf8("\x61\xf0\x80\x80\x41");
echo "Done\n";
?>
--EXPECT--
bool(false)
string(4) ""ab""
string(10) ""a\ufffdb""
bool(false)
string(14) "2261efbfbd6222"
bool(false)
string(4) ""aA""
string(10) ""a\ufffdA""
bool(false)
string(14) "2261efbfbd4122"
Done
