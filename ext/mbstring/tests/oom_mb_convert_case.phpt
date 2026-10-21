--TEST--
mb_convert_case() doesn't reach hard limit
--EXTENSIONS--
mbstring
--INI--
memory_limit=8M
--FILE--
<?php
// 2 MB == memory_limit / 4
mb_convert_case(str_repeat("\u{0390}", 1024 * 1024), MB_CASE_UPPER, "UTF-8");
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
