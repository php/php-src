--TEST--
iconv_mime_decode_headers() charset parameter length checks (CVE-2007-4840)
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--FILE--
<?php
$a = str_repeat("/", 9000000);
var_dump(iconv_mime_decode_headers("a", null, $a));
?>
--EXPECTF--

Warning: iconv_mime_decode_headers(): Charset parameter exceeds the maximum allowed length of %d characters in %s on line %d
bool(false)
