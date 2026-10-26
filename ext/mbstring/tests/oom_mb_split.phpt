--TEST--
mb_split() doesn't reach hard limit
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_split")) die("skip mbregex support unavailable");
?>
--INI--
memory_limit=8M
--FILE--
<?php
mb_split("a", str_repeat("a", 2 * 1024 * 1024)); // 2 MB == memory_limit / 4
echo "not reached\n";
?>
--EXPECTF--
Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
