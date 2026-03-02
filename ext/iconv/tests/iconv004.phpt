--TEST--
iconv_mime_encode() sanity cheeck.
--EXTENSIONS--
iconv
--INI--
input_encoding = ISO-8859-1
internal_encoding = ISO-8859-1
output_encoding = ISO-8859-1
--FILE--
<?php
var_dump(iconv_mime_encode('', ''));
var_dump(iconv_mime_encode('', '', array('line-break-chars' => 1)));
?>
--EXPECT--
string(19) ": =?ISO-8859-1?B??="
string(19) ": =?ISO-8859-1?B??="
