--TEST--
GH-17399 (iconv_mime_decode() leak on bailout)
--EXTENSIONS--
iconv
--INI--
memory_limit=2M
--FILE--
<?php
$str = str_repeat('a', 1024 * 1024);
iconv_mime_decode($str, 0, 'UTF-8');
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %s in %s on line %d
