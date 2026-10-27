--TEST--
mb_ereg_replace() doesn't reach hard limit
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_ereg_replace")) die("skip mbregex support unavailable");
?>
--INI--
memory_limit=8M
--FILE--
<?php
// 2 MB == memory_limit / 4
mb_ereg_replace("a", "bbbbbbbbbb", str_repeat("a", 2 * 1024 * 1024));
echo "not reached\n";
?>
--EXPECTF--
Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
