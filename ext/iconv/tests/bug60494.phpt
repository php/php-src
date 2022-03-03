--TEST--
Bug #60494 (iconv_mime_decode does ignore special characters)
--EXTENSIONS--
iconv
--FILE--
<?php
var_dump(iconv_mime_decode('ä'));
var_dump(iconv_mime_decode('ö'));
var_dump(iconv_mime_decode('ß'));
?>
--EXPECTF--
Notice: iconv_mime_decode(): Detected an illegal character in input string in %s on line %d
bool(false)

Notice: iconv_mime_decode(): Detected an illegal character in input string in %s on line %d
bool(false)

Notice: iconv_mime_decode(): Detected an illegal character in input string in %s on line %d
bool(false)
