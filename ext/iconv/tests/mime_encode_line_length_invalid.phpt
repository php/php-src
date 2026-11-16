--TEST--
iconv_mime_encode(): a non-positive line-length is rejected
--EXTENSIONS--
iconv
--FILE--
<?php
$o = ["input-charset" => "UTF-8", "output-charset" => "UTF-8"];
foreach ([-1, 0, PHP_INT_MIN] as $len) {
    var_dump(iconv_mime_encode("S", "héllo", $o + ["line-length" => $len]));
}
var_dump(iconv_mime_encode("S", "héllo", $o + ["line-length" => 76]));
?>
--EXPECTF--
Warning: iconv_mime_encode(): The "line-length" option must be greater than 0 in %s on line %d
bool(false)

Warning: iconv_mime_encode(): The "line-length" option must be greater than 0 in %s on line %d
bool(false)

Warning: iconv_mime_encode(): The "line-length" option must be greater than 0 in %s on line %d
bool(false)
string(%d) "S: =?UTF-8?B?%s?="
