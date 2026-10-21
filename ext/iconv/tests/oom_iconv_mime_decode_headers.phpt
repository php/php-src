--TEST--
iconv_mime_decode_headers() doesn't reach hard limit
--EXTENSIONS--
iconv
--INI--
memory_limit=8M
--FILE--
<?php
$headers = str_repeat("a:b\n", 512 * 1024); // 2 MB == memory_limit / 4
iconv_mime_decode_headers($headers, 0, "ISO-8859-1");
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
