--TEST--
mb_str_split() doesn't reach hard limit - fixed width encodings
--EXTENSIONS--
mbstring
--INI--
memory_limit=8M
--FILE--
<?php
// 2 MB == memory_limit / 4
mb_str_split(str_repeat("a", 2 * 1024 * 1024), 1, "ASCII");
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
