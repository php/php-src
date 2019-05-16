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
	$json = json_decode($str, true, 512, JSON_INVALID_UTF8_SUBSTITUTE);
	if (is_array($json)) {
		var_dump(array_map(function($item) { return bin2hex($item); }, $json));
	} else {
		var_dump(bin2hex($json));
	}
}
json_decode_invalid_utf8("\"a\xb0b\"");
json_decode_invalid_utf8("\"a\xd0\xf2b\"");
json_decode_invalid_utf8("\"\x61\xf0\x80\x80\x41\"");
json_decode_invalid_utf8("[\"\xc1\xc1\",\"a\"]");
echo "Done\n";
?>
--EXPECT--
NULL
string(2) "ab"
string(10) "61efbfbd62"
NULL
string(2) "ab"
string(16) "61efbfbdefbfbd62"
NULL
string(2) "aA"
string(22) "61efbfbdefbfbdefbfbd41"
NULL
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(1) "a"
}
array(2) {
  [0]=>
  string(12) "efbfbdefbfbd"
  [1]=>
  string(2) "61"
}
Done
