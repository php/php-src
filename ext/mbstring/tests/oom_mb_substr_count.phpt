--TEST--
mb_substr_count() doesn't reach hard limit
--EXTENSIONS--
mbstring
--INI--
memory_limit=8M
--FILE--
<?php
// 3 MB needle -> 12 MB wchar buffer, memory_limit is 8 MB
mb_substr_count("abc", str_repeat("a", 3 * 1024 * 1024));
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
