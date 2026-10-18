--TEST--
mb_convert_encoding() doesn't reach hard limit
--EXTENSIONS--
mbstring
--INI--
memory_limit=8M
--FILE--
<?php
// 2 MB == memory_limit / 4
mb_convert_encoding(str_repeat("a", 2 * 1024 * 1024), "UTF-32", "ISO-8859-1");
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
