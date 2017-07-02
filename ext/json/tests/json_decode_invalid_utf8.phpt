--TEST--
json_decode() invalid UTF8
--SKIPIF--
<?php
if (!extension_loaded("json")) print "skip";
?>
--FILE--
<?php
$str = "\"\xb0bar\"";
var_dump(json_decode($str));
var_dump(json_decode($str, true, 512, JSON_INVALID_UTF8_IGNORE));
var_dump(bin2hex(json_decode($str, true, 512, JSON_INVALID_UTF8_SUBSTITUTE)));
echo "Done\n";
?>
--EXPECTF--
NULL
string(3) "bar"
string(12) "efbfbd626172"
Done
