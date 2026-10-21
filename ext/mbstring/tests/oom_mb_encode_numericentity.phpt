--TEST--
mb_encode_numericentity() doesn't reach hard limit
--EXTENSIONS--
mbstring
--INI--
memory_limit=8M
--FILE--
<?php
// 2 MB == memory_limit / 4
$map = [0x0, 0x10FFFF, 0, 0xFFFFFF];
$string = str_repeat("a", 2 * 1024 * 1024);
mb_encode_numericentity($string, $map, "UTF-8", true);
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
