--TEST--
iconv_mime_encode() sanity cheeck.
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--FILE--
<?php
var_dump(iconv_mime_encode('', ''));
var_dump(iconv_mime_encode('', '', array('line-break-chars' => 1)));
?>
--EXPECT--
string(19) ": =?ISO-8859-1?B??="
string(19) ": =?ISO-8859-1?B??="
